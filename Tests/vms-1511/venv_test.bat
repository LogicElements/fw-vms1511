py -m venv .\venv --clear
call .\venv\Scripts\activate
pip install -r requirements.txt
py .\VisualVms1511.py
deactivate