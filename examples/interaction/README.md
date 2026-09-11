Example of a simple message:
    {"type": "ask", "title": "My first message", "message": "This is my first message !!"}

Example of getting a string:
    {"type": "ask", "title": "Unknown user", "message": "Please enter your first and last name", "responseType": "string"}

Example of getting a number:
    {"type": "ask", "title": "Data missing", "message": "Please enter your age", "responseType": "number"}

Example of getting a choice in a list:
    {"type": "ask", "buttons": ["Ok", "Cancel"], "responseType": {"list": ["Blue", "Green", "Red", "White"]}}
