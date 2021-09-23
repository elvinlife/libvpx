import sys
video = "mot17-10"
if len(sys.argv) == 2:
    video = sys.argv[1]

layer_size = [[], [], []]
layer_ssim = [[], [], []]
fname = video + "/" + video + ".log"
sname = video + "/" + video + "-ssim.log"

for i in range(3):
    with open( video + "/" + video + "-ssim" + str(i) + ".log", "r" ) as fin:
        for line in fin:
            words = line.split(" ")
            ssim = float( words[-1][1:-2] )
            layer_ssim[i].append( ssim )
with open( fname, "r" ) as fin:
    lines = fin.readlines()
for i, line in enumerate(lines):
    words = line.split(" ")
    if words[0] == "SVC":
        size1 = int( lines[i+1].split(" ")[-1] )
        size2 = int( lines[i+2].split(" ")[-1] )
        size3 = int( lines[i+3].split(" ")[-1] )
        layer_size[0].append( size1 )
        layer_size[1].append( size2 - size1 )
        layer_size[2].append( size3 - size2 )

with open( video + "/" + video + "-quality.trace", "w" ) as fout:
    fout.write( "10 3\n" )
    for i in range( len(layer_size[0]) ):
        fout.write( "6000 %d %d %d 1200 %.2f\n" % ( i, 0, layer_size[0][i], layer_ssim[0][i] ) )
        fout.write( "6000 %d %d %d 3000 %.2f\n" % ( i, 1, layer_size[1][i], layer_ssim[1][i] ) )
        fout.write( "6000 %d %d %d 6000 %.2f\n" % ( i, 2, layer_size[2][i], layer_ssim[2][i] ) )
