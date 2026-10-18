print("\n\n---------------------------------------\n")


from d3gl import D3GL, Colors
import math

d3gl = D3GL(320, 480 - 32)
print("d3gl init success")

d3gl.draw_line(10, 10, 50, 50, Colors.RED)

d3gl.update()
print("d3gl update success")


print("\n---------------------------------------\n\n")