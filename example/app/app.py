from flask import Flask, render_template, request, session, redirect, url_for

app = Flask(__name__)
app.secret_key = b'_5#y2L"F4Q8z\n\xec]/'

@app.route('/')
def index():
    if 'username' in session:
        return f'Logged in as {session["username"]}'
    return 'You are not logged in'

@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        session['username'] = request.form['username']
        return redirect(url_for('index'))
    return '''
        <form method="post">
            <p><input type=text name=username>
            <p><input type=submit value=Login>
        </form>
    '''

@app.route('/logout')
def logout():
    # remove the username from the session if it's there
    session.pop('username', None)
    return redirect(url_for('index'))

# Route for the GET method
@app.route('/test')
def test():
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
