from flask import Flask, render_template, request, session, redirect, url_for

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'

@app.route('/', methods=['GET','POST'])
def index():
    if 'username' in session:
        dict = {
            "Directory Listing" : "/",
            "Redirection" : "/redir",
            "Video" : "video",
            "Perl CGI" : "/cgi/perl.cgi",
            "Big Image" : "/html/example/big_image.jpg"
        }
        if request.method == 'GET':
            return render_template('index.html', name=session["username"], dict=dict)
        else:
            file = request.files['file']
            file.save(file.filename);
            return render_template(
                'index.html',
                name=session["username"],
                dict=dict,
                result="OK",
                filename=file.filename
            )
    return render_template('index.html')

@app.route('/video')
def video():
    if 'username' in session:
        return render_template('video.html', name=session["username"])
    return render_template('video.html')

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        session['username'] = request.form['username']
        return redirect(url_for('index'))
    return render_template("login.html")

@app.route('/logout')
def logout():
    session.pop('username', None)
    return redirect(url_for('index'))

@app.route('/test')
def test():
    return render_template('big.html')

@app.route('/upload/<name>', methods=['GET','POST'])
def submit(name: str):
    with open(name, "wb") as f:
        f.write(request.data)
    return render_template('big.html')

if __name__ == '__main__':
    app.run(debug=True)
