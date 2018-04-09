from flask import Flask, render_template, request
import os

app = Flask(__name__)
path = "/home/pi/files/"

@app.route("/")
def main():
    files = os.listdir(path)
    template_data = { 'files' : files }
    return render_template("main.html", **template_data)
    
@app.route("/", methods = ['POST'])
def create_file():
	file_name =  path + request.form['file_name']
	open(file_name, 'w+')
	return main()
    
if __name__ == "__main__":
    app.run(host = "0.0.0.0", port = 80, debug = True)
