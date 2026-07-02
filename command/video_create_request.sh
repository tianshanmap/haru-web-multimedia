curl -X POST http://localhost:8080/filesystem/video/generate \
     -H "Content-Type: application/json" \
     -d '{
      "video_name":"australia-trip",
      "image_path":"/Users/developer/T9/travels/processed/174-alison/jpeg",
      "audio_files":[
        "/Users/developer/T9/workshop/media-mp3/李云迪-沂蒙颂.mp3",
        "/Users/developer/T9/workshop/media-mp3/攙扶-F調-凡人薩克斯風.mp3",
        "/Users/developer/T9/workshop/media-mp3/洪湖水浪打浪.mp3"
        ]
      }'