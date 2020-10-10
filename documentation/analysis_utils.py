import requests
import pandas as pd
from pathlib import Path


# _BASE_URL = 'https://api.mplevy.com/api/mbot/v1/'

# DEBUG URL ONLY
_BASE_URL = 'http://127.0.0.1:8505/api/mbot/v1/'

# SPECIFIC URLs
_LOG_URL = _BASE_URL + 'log'
_DIRECTORY_URL = _BASE_URL + 'directory'


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
    r = requests.get(_LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text)
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
    
    if not isinstance(database, str):
        return -1
    elif database.lower() == 'prod':
        payload = {'database': 'prod'}
    elif database.lower() == 'backup':
        payload = {'database': 'backup'}
    else:
        return -1
    
    # get the data back unwrapped.
    r = requests.get(_DIRECTORY_URL, params=payload)
    
    if r.status_code != 200:
        print(r.status_code)
        print(r.text)
        return -1
    
    # open file
    file_path = Path(f'/tmp/mbot_table_{database}.pkl')

    # delete if it's already there (not working < 3.8)
    file_path.unlink(missing_ok=True)
    
    # read in content
    with open(file_path, 'wb') as fd:
        fd.write(r.content)
    
    # read in df
    df = pd.read_pickle(file_path)

    # delete file
    file_path.unlink(missing_ok=True)
    return df

# must give dir/name
def get_log(runId, dir_name):
    '''Give a runId from database and save the log file'''
    if isinstance(runId, str):
        runId = int(runId)
    elif not isinstance(runId, int):
        return -1
    
    # pass runId into payload
    payload = {'runId': runId, 'type': 'log'}

    # get the data back unwrapped.
    r = requests.get(_LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text())
        return -1
    
    # open file
    file_path = Path(dir_name)

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
    r = requests.delete(_LOG_URL, params=payload)
    if r.status_code != 200:
        print(r.text())
        return -1
    else:
        return 0


def post_log(botname, description, path):
    '''pass in path string'''
    # read file
    file_path = Path(path)
    
    # check if correct path
    if not file_path.is_file():
        return -1


    # open file
    file = open(file_path, 'rb')

    # create payload
    payload = {'logfile': file}
    param = {'name': botname, 'description': description}
    # post file
    r = requests.post(_LOG_URL, params=param, files=payload)
    
    # close file
    file.close()
    if r.status_code != 200: 
        print(r.text)
        return -1
    else: 
        return r.json()


def test_get_table():
    print('\n**********TEST GET PRODUCTION TABLE!**********\n')
    df = get_table('prod')
    if isinstance(df, int):
        print(df)
        return -1
    else:
        print(df.loc[0])
        print(df.head())
    
    print('\n**********TEST GET BACKUP TABLE!**********\n')
    df = get_table('backup')
    if isinstance(df, int):
        print(df)
        return -1
    else:
        print(df.loc[0])
        print(df.head())
        return 0


def test_get_df(runId):
    print('\n**********TEST GET DF!**********\n')
    df = get_df(runId)
    if isinstance(df, int):
        print(df)
        return -1
    else:
        print(df.keys())
        return 0


def test_get_log():
    print('\n**********TEST GET LOG!**********\n')
    path = '/tmp/mbot_test.log'
    if not get_log(0, path):
        return 0
    else:
        return -1
        
def test_post_log():
    print('\n**********TEST POST LOG!**********\n')
    botname = 'test'
    description = 'test'
    path = '../../MBOT-RPI/data/convex_10mx10m_5cm.log'
    results = post_log(botname, description, path)
    return results


if __name__ == '__main__':
    # test lookup table
    test_get_table()
    
    # test get df
    test_get_df(0)

    # test get log
    test_get_log()

    # test post log
    results = test_post_log()
    print('\n**********DELETE LATEST RUN!**********\n')
    if not isinstance(results, int):
        r = delete_run(results['runId'])
        if r == 0:
            print('\n********DELETED TEST RUN********\n')
        else:
            print('\n********FAILED TO DELETE TEST RUN*******\n')

