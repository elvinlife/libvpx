#!/bin/bash
VIDEO="mot17-10"
INPUT="MOT17-10-FRCNN-raw.webm"

# generate the uncompressed yuv video
../vpxdec --rawvideo ../video-source/$INPUT -o ${VIDEO}/${VIDEO}-origin.yuv

# encode the uncompressed yuv into three quality layers with same resolutions
# the cumulative bitrate is 6000kbps, and the layers within are 1200kbps, 1800kbps, and 3000kbps
../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 6000 -sl 3 -k 100 --scale-factors=960/960,960/960,960/960 -bl 1200,1800,3000  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO} &> ${VIDEO}/${VIDEO}.log

# calculate the ssim of every frame decoded with only one layer
../vpxdec --rawvideo ${VIDEO}/${VIDEO}_s0.ivf -o ${VIDEO}/${VIDEO}-out.yuv
ffmpeg -s 960x540 -i ${VIDEO}/${VIDEO}-out.yuv -s 960x540 -i ${VIDEO}/${VIDEO}-origin.yuv -lavfi ssim="stats_file=${VIDEO}/${VIDEO}-ssim0.log" -f null -

# calculate the ssim of every frame decoded with two layers
../vpxdec --rawvideo ${VIDEO}/${VIDEO}_s1.ivf -o ${VIDEO}/${VIDEO}-out.yuv
ffmpeg -s 960x540 -i ${VIDEO}/${VIDEO}-out.yuv -s 960x540 -i ${VIDEO}/${VIDEO}-origin.yuv -lavfi ssim="stats_file=${VIDEO}/${VIDEO}-ssim1.log" -f null -

# calculate the ssim of every frame decoded with all layers
../vpxdec --rawvideo ${VIDEO}/${VIDEO}_s2.ivf -o ${VIDEO}/${VIDEO}-out.yuv
ffmpeg -s 960x540 -i ${VIDEO}/${VIDEO}-out.yuv -s 960x540 -i ${VIDEO}/${VIDEO}-origin.yuv -lavfi ssim="stats_file=${VIDEO}/${VIDEO}-ssim2.log" -f null -

# generate the video trace
python3 gen-svc-trace.py

rm ${VIDEO}/*.yuv
rm ${VIDEO}/${VIDEO}
