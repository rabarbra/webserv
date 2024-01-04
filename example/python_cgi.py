#!/Users/psimonen/.brew/bin/python3
from wsgiref.handlers import CGIHandler
from app import app
import os
import json

with open("test_output.json", "w") as logfile:
    logfile.write(json.dumps(dict(os.environ)))
CGIHandler().run(app)
