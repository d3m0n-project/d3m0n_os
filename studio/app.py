from flask import Flask, jsonify, send_from_directory, Response
from pathlib import Path
import os
from PIL import Image
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


@app.route("/")
def index():
    return send_from_directory(BASE_DIR, "index.html")


@app.route("/style.css")
def style():
    return send_from_directory(BASE_DIR, "style.css")


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
    # Images: resize then return base64
    if path.suffix.lower() in IMAGE_EXTENSIONS:
        try:
            with Image.open(path) as img:
                img.thumbnail((512, 512))  # Max width/height

                # Convert unsupported modes
                if img.mode not in ("RGB", "RGBA"):
                    img = img.convert("RGB")

                buffer = io.BytesIO()

                # JPEG is much smaller for photos
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


@app.route("/api/applications")
def applications():
    apps = []

    if not APPS_DIR.exists():
        return jsonify([])

    # Find every file named "app" anywhere under ../rootfs/apps
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
            "manifest": manifest,
            "files": hierarchy
        })

    return jsonify(apps)


if __name__ == "__main__":
    app.run(host="127.0.0.1", port=8000, debug=True)