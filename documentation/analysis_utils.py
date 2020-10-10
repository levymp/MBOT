import requests
import pandas as pd
from pathlib import Path

# BASE_URL = 'https://api.mplevy.com/api/mbot/v1/'

#### DEBUG URL ONLY
BASE_URL = 'http://127.0.0.1:8505/api/mbot/v1/'

# SPECIFIC URLs
LOG_URL = BASE_URL + 'log'
DIRECTORY_URL = BASE_URL + 'directory'




# be sure to give dir/name.pkl if you plan to save (and set save=True)
def get_df(runId, name='/tmp/mbot_temp.pkl', save=False):
    '''GET A pandas df from MBOT Database and either don't save it or save it (rename directory in that case)'''
    if isinstance(runId, str):
        runId = int(runId)
    elif not isinstance(runId, int):
        return -1
    
    # pass runId into payload
    payload = {'runId': runId, 'type': 'pkl'}

    # get the data back unwrapped.
    r = requests.get(LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text())
        return -1
    
    # open file
    file_path = Path(name)

    # delete if it's already there
    # file_path.unlink(missing_ok=True)
    
    # read in content
    with open(file_path, 'wb') as fd:
        fd.write(r.content)
    
    # read in df
    df = pd.read_pickle(file_path)

    # delete file
    if save is False:
        file_path.unlink()
    return df

def get_table(database):
    '''GET the lookup table for prod or backup database'''
    
    if isinstance(database, str):
        return -1
    elif database.lower() == 'prod':
        payload = {'database' = 'prod'}
    elif database.lower() == 'backup':
        payload = {'database' = 'backup'}
    else:
        return -1
    
    # get the data back unwrapped.
    r = requests.get(DIRECTORY_URL, params=payload)
    
    if r.status_code != 200:
        print(r.text())
        return -1
    
    # open file
    file_path = Path('/tmp/mbot_table.pkl')

    # delete if it's already there
    # file_path.unlink(missing_ok=True)
    
    # read in content
    with open(file_path, 'wb') as fd:
        fd.write(r.content)
    
    # read in df
    df = pd.read_pickle(file_path)

    # delete file
    file_path.unlink()
    return df

# must give dir/name
def get_log(runId, name):
    '''Give a runId from database and save the log file'''
    if isinstance(runId, str):
        runId = int(runId)
    elif not isinstance(runId, int):
        return -1
    
    # pass runId into payload
    payload = {'runId': runId, 'type': 'log'}

    # get the data back unwrapped.
    r = requests.get(LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text())
        return -1
    
    # open file
    file_path = Path(name)

    # delete if it's already there
    file_path.unlink(missing_ok=True)
    
    # read in content/save
    with open(file_path, 'wb') as fd:
        fd.write(r.content)
    return 0


def delete_run(runId):
    '''Delete a run'''
    if isinstance(runId, str):
        runId = int(runId)
    elif not isinstance(runId, int):
        return -1
    payload = {'runId': runId}
    r = requests.delete(LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text())
        return -1
    else:
        return 0


def post_log(name, path):
    '''pass in path string'''
    # read file
    file_path = Path(path)
    
    # check if correct path
    if not file_path.is_file():
        return -1
    if name not in ['MICHAEL', 'SAM', 'HAMIL', 'XUN', '-']:
        return -1

    # open file
    file = open(file_path, 'rb')

    # create payload
    payload = {'logfile': file}
    param = {'name': name}
    # post file
    r = requests.post(LOG_URL, params=param, files=payload)
    
    # close file
    file.close()
    if r.status_code != 200: 
        print(r.text)
        return -1
    else: 
        return r.json()


if __name__ == '__main__':
    df = get_table('backup')




