import flask

app = flask.Flask(__name__)

@app.route('/test', methods=['GET'])
def home():
    return "Hello World!"

if __name__ == '__main__':
    app.run(debug=True)
