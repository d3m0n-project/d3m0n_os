from flask import Flask, jsonify, send_from_directory, Response, request
from pathlib import Path
import os
import re
import shutil
import subprocess
from PIL import Image
from cpp_ast import parse_cpp
import base64
import io

IMAGE_EXTENSIONS = {
    ".png", ".jpg", ".jpeg", ".gif",
    ".bmp", ".webp", ".tiff", ".ico"
}

app = Flask(__name__)

BASE_DIR = Path(__file__).resolve().parent
ROOTFS_CONFIG = (BASE_DIR / "../rootfs/config").resolve()
APPS_DIR = (BASE_DIR / "../rootfs/apps").resolve()
TEMPLATE_DIR = (BASE_DIR / "../compiler/examples/simple_app").resolve()
icon_pack = None


@app.route("/")
def index():
    return send_from_directory(BASE_DIR, "index.html")


@app.route("/style.css")
def style():
    return send_from_directory(BASE_DIR, "style.css")


@app.route("/favicon.ico")
def icon():
    return send_from_directory(BASE_DIR, "favicon.ico")


@app.route("/app.js")
def app_js():
    return send_from_directory(BASE_DIR, "app.js")


@app.route("/api/config")
def config():
    if not ROOTFS_CONFIG.exists():
        return Response("Config file not found", status=404)

    return Response(
        ROOTFS_CONFIG.read_text(encoding="utf-8"),
        mimetype="text/plain",
    )


def build_tree(path: Path):
    result = [path.name]

    for item in sorted(path.iterdir(), key=lambda p: (p.is_file(), p.name)):
        if item.is_dir():
            result.append(build_tree(item))
        else:
            result.append((item.name, read_file(item)))
    return result


def read_file(path: Path):
    if path.suffix.lower() in IMAGE_EXTENSIONS:
        try:
            with Image.open(path) as img:
                img.thumbnail((512, 512))

                if img.mode not in ("RGB", "RGBA"):
                    img = img.convert("RGB")

                buffer = io.BytesIO()

                if path.suffix.lower() in {".jpg", ".jpeg"}:
                    img.save(buffer, format="JPEG", quality=75, optimize=True)
                    mime = "image/jpeg"
                else:
                    img.save(buffer, format="PNG", optimize=True)
                    mime = "image/png"

                encoded = base64.b64encode(buffer.getvalue()).decode("ascii")

                return {
                    "type": "image",
                    "mime": mime,
                    "data": encoded
                }
        except Exception:
            pass

    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return {
            "type": "binary",
            "size": path.stat().st_size
        }


def project_path(project_id):
    if not isinstance(project_id, str) or project_id.startswith("/"):
        return None
    path = (APPS_DIR / project_id).resolve()
    if APPS_DIR not in path.parents or any(part in {"", ".", ".."} for part in Path(project_id).parts) or not path.is_dir():
        return None
    return path


def project_files(path: Path):
    result = {}
    for item in sorted(path.rglob("*")):
        if item.is_file() and item.name != "app":
            result[str(item.relative_to(path))] = read_file(item)
    return result


@app.route("/api/icons/<path:filename>")
def icons(filename):
    global icon_pack
    if not icon_pack:
        if not ROOTFS_CONFIG.exists():
            return Response("Config file not found", status=404)

        lines = ROOTFS_CONFIG.read_text(encoding="utf-8").split('\n')
        for line in lines:
            line = line.strip()
            k, v = tuple([p.strip() for p in line.split(':')])
            if k == "icon_pack":
                icon_pack = v
    if not icon_pack or '.' in icon_pack or '/' in icon_pack:
        return Response("icon_pack not found", status=404)
    return send_from_directory(f"../rootfs/theme/{icon_pack}/", filename)


@app.route("/api/applications")
def applications():
    apps = []

    if not APPS_DIR.exists():
        return jsonify([])

    for manifest_path in APPS_DIR.rglob("app"):
        if not manifest_path.is_file():
            continue

        app_dir = manifest_path.parent

        manifest = manifest_path.read_text(
            encoding="utf-8",
            errors="replace"
        )

        hierarchy = []
        for item in sorted(app_dir.iterdir(), key=lambda p: (p.is_file(), p.name)):
            if item.is_dir():
                hierarchy.append(build_tree(item))
            else:
                hierarchy.append((
                    item.name,
                    read_file(item)
                ))

        apps.append({
            "id": str(app_dir.relative_to(APPS_DIR)),
            "manifest": manifest,
            "files": hierarchy,
            "project": app_dir.name
        })
    return jsonify(apps)


@app.post("/api/applications")
def create_application():
    payload = request.get_json(silent=True) or {}
    name = str(payload.get("name", "")).strip()
    if not re.fullmatch(r"[A-Za-z0-9_.-]+", name):
        return jsonify({"error": "Use only letters, numbers, dots, dashes, and underscores."}), 400
    if not TEMPLATE_DIR.is_dir():
        return jsonify({"error": "The simple_app template is missing."}), 500
    APPS_DIR.mkdir(parents=True, exist_ok=True)
    destination = (APPS_DIR / name).resolve()
    if destination.parent != APPS_DIR or destination.exists():
        return jsonify({"error": "That application already exists."}), 409
    shutil.copytree(TEMPLATE_DIR, destination, ignore=shutil.ignore_patterns("obj", "*.o", "*.d"))
    (destination / "app").write_text(
        f"name: {name}\nicon: icon.ico\ndescription: Studio application\n",
        encoding="utf-8",
    )
    return jsonify({"id": name}), 201


@app.delete("/api/applications/<path:project_id>")
def delete_application(project_id):
    path = project_path(project_id)
    if not path:
        return jsonify({"error": "Application not found."}), 404
    shutil.rmtree(path)
    return jsonify({"ok": True})


def project_target(path, relative):
    if not isinstance(relative, str) or not relative.strip():
        return None
    target = (path / relative).resolve()
    if target == path or path not in target.parents or target.name == "app":
        return None
    return target


@app.post("/api/applications/<path:project_id>/files/operation")
def operate_application_file(project_id):
    path = project_path(project_id)
    payload = request.get_json(silent=True) or {}
    action = payload.get("action")
    source = project_target(path, payload.get("path")) if path else None
    destination = project_target(path, payload.get("destination")) if path and payload.get("destination") else None
    if not path or not source or action not in {"create", "delete", "rename", "copy"}:
        return jsonify({"error": "Invalid file operation."}), 400
    if action == "create":
        if source.exists():
            return jsonify({"error": "That file or folder already exists."}), 409
        source.parent.mkdir(parents=True, exist_ok=True)
        if payload.get("kind") == "folder":
            source.mkdir()
        else:
            source.write_text(str(payload.get("content", "")), encoding="utf-8")
    elif action == "delete":
        if not source.exists():
            return jsonify({"error": "File or folder not found."}), 404
        if source.is_dir():
            shutil.rmtree(source)
        else:
            source.unlink()
    elif action == "rename":
        if not destination or not source.exists() or destination.exists():
            return jsonify({"error": "Invalid rename target."}), 400
        destination.parent.mkdir(parents=True, exist_ok=True)
        source.rename(destination)
    elif action == "copy":
        if not destination or not source.exists() or destination.exists():
            return jsonify({"error": "Invalid copy target."}), 400
        destination.parent.mkdir(parents=True, exist_ok=True)
        if source.is_dir():
            shutil.copytree(source, destination)
        else:
            shutil.copy2(source, destination)
    return jsonify({"ok": True})


@app.post("/api/applications/<path:project_id>/files/parse")
def parse_application_file(project_id):
    if not project_path(project_id):
        return jsonify({"error": "Application not found."}), 404
    payload = request.get_json(silent=True) or {}
    content = payload.get("content")
    if not isinstance(content, str):
        return jsonify({"error": "Invalid C++ source."}), 400
    try:
        return jsonify({"model": parse_cpp(content)})
    except Exception as error:
        return jsonify({"error": str(error)}), 422


@app.get("/api/applications/<path:project_id>/files")
def application_files(project_id):
    path = project_path(project_id)
    if not path:
        return jsonify({"error": "Application not found."}), 404
    return jsonify(project_files(path))


@app.post("/api/applications/<path:project_id>/files")
def save_application_file(project_id):
    path = project_path(project_id)
    payload = request.get_json(silent=True) or {}
    relative = payload.get("path")
    content = payload.get("content")
    if not path or not isinstance(relative, str) or not isinstance(content, str):
        return jsonify({"error": "Invalid application file."}), 400
    target = (path / relative).resolve()
    if target.parent == path / "app" or path not in target.parents or target.name == "app":
        return jsonify({"error": "Invalid application file path."}), 400
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(content, encoding="utf-8")
    parsed = None
    if target.suffix.lower() in {".cpp", ".hpp", ".h", ".cc", ".cxx"}:
        try:
            parsed = parse_cpp(content)
        except Exception:
            parsed = None
    return jsonify({"ok": True, "model": parsed})


@app.post("/api/applications/<path:project_id>/compile")
def compile_application(project_id):
    path = project_path(project_id)
    if not path:
        return jsonify({"error": "Application not found."}), 404
    try:
        result = subprocess.run(
            ["make"], cwd=path, capture_output=True, text=True, timeout=180
        )
    except subprocess.TimeoutExpired as error:
        return jsonify({"ok": False, "output": f"Compilation timed out after 180 seconds.\n{error.stdout or ''}"}), 504
    output = (result.stdout or "") + (result.stderr or "")
    return jsonify({"ok": result.returncode == 0, "returncode": result.returncode, "output": output})


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8000, debug=True)
