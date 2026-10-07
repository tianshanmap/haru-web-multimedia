#docker run -d -p 127.0.0.1:9082:8080 -v /Users/developer/docker-workshop/media-mp3:/app/workshop/media-mp3 ubuntu-ffmpeg:latest
docker run -d -p 127.0.0.1:9082:8080 --mount type=bind,source=/Users/developer/docker-workshop/media-mp3,target=/app/workshop/media-mp3 ubuntu-ffmpeg:latest
