Import("env")

# Rename littlefs.bin to fs.bin
env.Replace(ESP8266_FS_IMAGE_NAME="fs")