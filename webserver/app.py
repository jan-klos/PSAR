from flask import Flask, render_template, request, redirect, send_file
import socket  # for IP address testing
import os
import util
import settings

app = Flask(__name__)

if not os.path.exists(settings.FILES_PATH):
    os.makedirs(settings.FILES_PATH)

# folder for temporary image files storing
util.create_tmp()


@app.route("/")
def main(error=""):
    util.clear_tmp()  # removes temporary image files
    file_list = os.listdir(settings.FILES_PATH)
    file_dict_list = util.get_file_dict_list(file_list)
    template_data = {"files": file_dict_list}
    return render_template("main.html", error=error, **template_data)


@app.route("/", methods=["POST"])
def add_file():
    error = ""
    if "create_file_submit" in request.form:
        if request.form["file_name"] == "":
            error = "You must name the file"
        else:
            util.create_file(request.form["file_name"], request.form["file_content"])
    elif "upload_file_submit" in request.form:
        if not request.files.get("file", None):
            error = "You must choose a file"
        else:
            util.upload_file(request.files["file"])
    return main(error=error)


@app.route("/files/<file_name>", methods=["POST"])
def my_send_file(file_name):
    if "send_file_submit" in request.form:
        send_address = request.form["send_address"]
        try:
            socket.inet_pton(socket.AF_INET, send_address)
        except socket.error:
            return show_file(file_name, error="Incorrect IP address")
        util.write_send_info(send_address, file_name)
    elif "download_file_submit" in request.form:
        return send_file(settings.FILES_PATH + file_name, attachment_filename=file_name, as_attachment=True)
    return redirect("/", code=302)


@app.route("/files/<file_name>", methods=["GET"])
def show_file(file_name, error=""):
    file_dict = util.get_file_dict(file_name)
    if file_dict["type"] in ("img", "pdf"):
        util.copy_file_to_tmp(file_name)
        template_data = {"file": file_dict}
    else:
        template_data = {"file": file_dict, "file_content": util.get_file_content(file_name)}
    return render_template("file.html", error=error, **template_data)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80, debug=True)
