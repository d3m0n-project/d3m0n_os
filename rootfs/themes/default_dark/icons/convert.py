from PIL import Image
import os

for file in os.listdir("."):
    if file.lower().endswith(".png"):
        with Image.open(file) as img:
            img = img.convert("RGBA")
            img = img.resize((100, 100))
            new_name = os.path.splitext(file)[0] + ".bmp"
            img.save(new_name, "BMP")