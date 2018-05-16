#!/usr/bin/env python

from flask import Flask, render_template, request, redirect, send_file
import socket  # for IP address testing
import os
import util
import settings
import messages

app = Flask(__name__)
f = util.Foo()

if not os.path.exists(settings.FILES_PATH):
    os.makedirs(settings.FILES_PATH)

# folder for temporary image files storing
util.create_tmp()


@app.route("/")
def main(error="", conf=""):
    util.clear_tmp()  # removes temporary image files
    file_list = os.listdir(settings.FILES_PATH)
    file_dict_list = util.get_file_dict_list(file_list)
    template_data = {"files": file_dict_list}
    return render_template("main.html", error=error, conf=conf, **template_data)


@app.route("/", methods=["POST"])
def add_file():
    error, conf = "", ""
    if "create_file_submit" in request.form:
        if request.form["file_name"] == "":
            error = messages.name_file_err;
        else:
            util.create_file(request.form["file_name"], request.form["file_content"])
            conf = messages.file_created_conf;
    elif "upload_file_submit" in request.form:
        if not request.files.get("file", None):
            error = messages.choose_file_err;
        else:
            util.upload_file(request.files["file"])
            conf = messages.file_uploaded_conf;
    return main(error=error, conf=conf)


@app.route("/files/<file_name>", methods=["POST"])
def my_send_file(file_name):
    if "send_file_submit" in request.form:
        send_address = request.form["send_address"]
        try:
            socket.inet_pton(socket.AF_INET, send_address)
        except socket.error:
            return show_file(file_name, error=messages.ip_addr_err)
        util.write_send_info(send_address, file_name)
        f.bar()
    elif "download_file_submit" in request.form:
        return send_file(settings.FILES_PATH + file_name, attachment_filename=file_name, as_attachment=True)
    return show_file(file_name, conf=messages.file_sent_conf)


@app.route("/files/<file_name>", methods=["GET"])
def show_file(file_name, error="", conf=""):
    file_dict = util.get_file_dict(file_name)
    if file_dict["type"] in ("img", "pdf"):
        util.copy_file_to_tmp(file_name)
        template_data = {"file": file_dict}
    else:
        template_data = {"file": file_dict, "file_content": util.get_file_content(file_name)}
    return render_template("file.html", error=error, conf=conf, **template_data)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80, debug=True)
