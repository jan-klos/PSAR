import os 
FILES_PATH = "/home/pi/files/"
SEND_FILES_PATH = "/home/pi/send_info.txt"
CURRENT_PATH = os.path.dirname(os.path.abspath(__file__))
ALLOWED_EXTENSIONS = set(['txt', 'pdf', 'png', 'jpg', 'jpeg', 'gif', 'mp3', "flac", "mp4", "avi", "webm", "gif", "pdf"])
LISTEN_FILE = "listen_file"
LISTEN_IP = "0.0.0.0"