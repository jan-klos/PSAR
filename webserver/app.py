from flask import Flask, render_template, request
import os

app = Flask(__name__)
path = "/home/pi/files/"

if not os.path.exists(path):
    os.makedirs(path)

@app.route("/")
def main():
    files = os.listdir(path)
    template_data = { "files" : files }
    return render_template("main.html", **template_data)
    
@app.route("/", methods = ["POST"])
def create_file():
	file_name =  path + request.form["file_name"]
	file_content = request.form["file_content"]
	file = open(file_name, "w+")
	file.write(file_content)	
	return main()

@app.route("/<file_name>", methods = ["GET"])
def print_file(file_name):
    with open(path + file_name) as file:
        file_content = file.read().replace("\n", "<br>")
    template_data = { "file_name" : file_name, "file_content" : file_content }
    return render_template("file.html", **template_data)
    
if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 80, debug = True)
