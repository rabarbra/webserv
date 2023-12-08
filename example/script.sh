#!/Users/psimonen/.brew/bin/python3
from wsgiref.handlers import CGIHandler
from app import app
import os

with open("test_output.txt", "w") as logfile:
    logfile.write(str(os.environ))
CGIHandler().run(app)
