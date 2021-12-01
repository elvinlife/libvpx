#!/bin/bash
VIDEO="mot17-10"
INPUT="MOT17-10-FRCNN-raw.webm"

# generate the uncompressed yuv video
../vpxdec --rawvideo ../video-source/$INPUT -o ${VIDEO}/${VIDEO}-origin.yuv

# encode the uncompressed yuv into three quality layers with same resolutions
# the cumulative bitrate is 6000kbps, and the layers within are 1200kbps, 1800kbps, and 3000kbps

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 1200 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 240,360,600  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_1200 &> ${VIDEO}/${VIDEO}_1200.log

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 2000 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 400,600,1000  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_2000 &> ${VIDEO}/${VIDEO}_2000.log

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 3000 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 600,900,1500  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_3000 &> ${VIDEO}/${VIDEO}_3000.log

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 4000 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 800,1200,2000  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_4000 &> ${VIDEO}/${VIDEO}_4000.log

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 5000 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 1000,1500,2500  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_5000 &> ${VIDEO}/${VIDEO}_5000.log

../examples/vp9_spatial_svc_encoder -w 960 -h 540 -rcstat 1 --rc-end-usage=1 -t 1/30 -b 6000 -sl 3 -k 10 --scale-factors=960/960,960/960,960/960 -bl 1200,1800,3000  ./mot17-10/mot17-10-origin.yuv -o ${VIDEO}/${VIDEO}_6000 &> ${VIDEO}/${VIDEO}_6000.log

