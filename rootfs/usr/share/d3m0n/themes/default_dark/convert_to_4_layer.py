from PIL import Image
import os

def convert_to_4_channel(image_path, output_path):
    # Open the image
    image = Image.open(image_path).convert("RGBA")
    
    # Save the new image
    image.save(output_path)
    print(f"Image saved to {output_path} with 4 channels.")





directory = './icons'

for filename in os.listdir(directory):
    f = os.path.join(directory, filename)
    if os.path.isfile(f):   
        convert_to_4_channel(f, f)