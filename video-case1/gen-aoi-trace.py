import sys
video = "mot17-10"
if len(sys.argv) == 2:
    video = sys.argv[1]

frame_size = []
frame_ssim = []
fname = video + "/" + video + ".log"
sname = video + "/" + video + "-ssim.log"
with open( fname, "r" ) as fin:
    for line in fin:
        words = line.split(" ")
        if words[0] == "stream:" and words[1] == "2":
            frame_size.append( int( words[-1][:-1] ) )
with open( sname, "r" ) as fin:
    for line in fin:
        words = line.split(" ")
        ssim = float( words[-1][1:-2] )
        frame_ssim.append( ssim )
with open( video + "/" + video + "-temp.trace", "w" ) as fout:
    fout.write( "8\n" )
    for i, fsize in enumerate( frame_size ):
        fout.write( "%d %d %.3f\n" % (i, fsize, frame_ssim[i] ) )
