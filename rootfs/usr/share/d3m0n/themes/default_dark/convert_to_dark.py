from PIL import Image
import os



directory = './icons'
 
for filename in os.listdir(directory):
    f = os.path.join(directory, filename)
    if f.endswith("-white.png") and os.path.isfile(f):   
        with Image.open(f) as img:
            #show the original image
            # grayimg.getpixel((0,0))
            rgbimage=img.convert('RGBA')
            for y in range(100):
                for x in range(100):
                    r, g, b, a=rgbimage.getpixel((x,y))
                    # print(a)
                    if r==255 and g==255 and b==255 and not a==0:
                        rgbimage.putpixel( (x,y), (0,0,0, a) )
                    
                    

            # grayscaleImg = img.convert("L")

            #convert into grayscale
            # = img.convert("L")
             
            #show the grayscale image
            rgbimage.save(f.replace("-white", "-black"))