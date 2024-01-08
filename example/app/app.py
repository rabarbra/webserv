from flask import Flask, render_template, request

app = Flask(__name__)

# Route for the GET method
@app.route('/test')
def index():
    return render_template('index.html')

@app.route('/upload/<name>', methods=['GET','POST'])
def submit(name: str):
    with open(name, "wb") as f:
        f.write(request.data)
    return render_template('index.html')

@app.route('/form_upload', methods=['GET','POST'])
def form():
    if request.method == 'GET':
        return render_template("post.html")
    else:
        file = request.files['file']
        file.save(file.filename);
        return render_template("post.html", result="OK", name=file.filename)

if __name__ == '__main__':
    app.run(debug=True)
