Import("env")
env.Replace(
  MKSPIFFSTOOL=env.get("PROJECT_DIR") + '/updateFS/UpdateLittleFSESP32'
)