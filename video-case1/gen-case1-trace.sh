#!/bin/bash
# mot17-10: 1200, 2500, 5000
# mot17-11: 1700, 3500, 7000
# mot17-12: 1500, 3000, 6000
VIDEO="mot17-10"
INPUT="MOT17-10-FRCNN-raw.webm"

# generate the uncompressed yuv video
../vpxdec --rawvideo ../video-source/$INPUT -o ${VIDEO}/${VIDEO}-origin.yuv

# encode the uncompressed video into three temporal layers
# check "../example/vpx_temporal_svc_encode -h" for more
../examples/vpx_temporal_svc_encoder ${VIDEO}/${VIDEO}-origin.yuv ${VIDEO}/${VIDEO} vp8 960 540 1 30 4 0 1 4 4 1700 3500 7000 &> ${VIDEO}/${VIDEO}.log

../vpxdec --rawvideo ${VIDEO}/${VIDEO}_2.ivf -o ${VIDEO}/${VIDEO}-out.yuv
# calculate the ssim of every frame
ffmpeg -s 960x540 -i ${VIDEO}/${VIDEO}-out.yuv -s 960x540 -i ${VIDEO}/${VIDEO}-origin.yuv -lavfi ssim="stats_file=${VIDEO}/${VIDEO}-ssim.log" -f null -

# generate the video trace
python3 gen-aoi-trace.py $VIDEO
rm ${VIDEO}/*.yuv
