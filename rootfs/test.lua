print("\n\n------------------------")

local d3gl = require("d3gl")

-- Create a 320 x 200 graphical surface.
d3gl.init(320, 480 - 32)

-- Draw a few pixels.
d3gl.putpixel(10, 10, d3gl.Colors.RED)
d3gl.putpixel(11, 10, d3gl.Colors.GREEN)
d3gl.putpixel(12, 10, d3gl.Colors.BLUE)

-- Display the changes.
d3gl.update()


print("------------------------\n\n")