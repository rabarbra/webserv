from flask import Flask, render_template, request

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

# Route for the GET method
@app.route('/test')
def test():
    return render_template('index.html')

# Route for the POST method
@app.route('/submit', methods=['GET','POST'])
def submit():
    # Access form data
    name = "vasilis"
    return render_template('submit.html', name=name)

if __name__ == '__main__':
    app.run(debug=True)

