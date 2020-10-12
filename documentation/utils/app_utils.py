from pathlib import Path
import pandas as pd
from datetime import datetime, timedelta
import api_utils as utils

def get_df(database):
    # if on linux box this will work
    # if not then running locally and need to use api
    try:
        path = Path(f'/home/michaellevy/data/prod/{database}/mbot_table.pkl')    
        df = pd.read_pickle(path)
    except Exception:
        df = utils.get_table(database)

    # get botnames
    botnames = list(df['BOT NAME'].unique())
    # add no filter option
    botnames.insert(0, 'NO FILTER')

    return df, botnames


def _get_indexes(df):
    index_list = range(len(df))
    index_list = ["RUN " + str(i) for i in index_list]
    return index_list


def filter_df(df, dates, botname):
    # check if one date or same date
    if len(dates) == 1:
        dates = (dates[0], dates[0] + timedelta(days=1))
    elif dates[0] == dates[1]:
        dates = (dates[0], dates[0] + timedelta(days=1))
    else:
        dates = (dates[0], dates[1] + timedelta(days=1))
    
    # add runs
    df = df.assign(Runs=_get_indexes(df))

    # check if filter by date or filter by date and botname
    if botname == 'NO FILTER':
        df = df[(df['DATE'] >= dates[0].strftime('%Y-%m-%d-%H:%M:%S')) &
                (df['DATE'] <= dates[1].strftime('%Y-%m-%d-%H:%M:%S'))]
    else:
        df = df[(df['DATE'] >= dates[0].strftime('%Y-%m-%d-%H:%M:%S')) &
                (df['DATE'] <= dates[1].strftime('%Y-%m-%d-%H:%M:%S')) &
                (df['BOT NAME'] == botname)]
    return df


# display all keys
def get_lookup(df):
    # start
    lookup = {}
    values = []
    weights = []
    # go through all keys
    for key1 in df.keys():
        for key2 in df[key1].keys():
            # append all keys to a list 
            values.append(key2)
        # add list of keys to lcm type dictionary
        lookup[key1] = values.copy()
        weights.append(len(values))
        # clear values
        values.clear()
    # write to pandas df
    df_keys = pd.DataFrame.from_dict(lookup, orient='index')

    # transpose columns
    df_keys = df_keys.transpose()
    
    # get weights lookup dictionary
    weights = dict(zip(df_keys.keys(), weights))
    
    # get columns
    cols = list(df_keys.keys())
    
    # sort columns
    cols.sort(key=lambda x: weights[x], reverse=True)
    
    # reorder dataframe
    df_keys = df_keys[cols]
    
    return df_keys

def file_to_time(df, runId, dateobj=True):
    time = df.loc[runId]['PICKLE NAME'].split('.')[0]
    time = datetime.strptime(time, '%Y_%m_%d_%H:%M:%S')
    if dateobj == True:
        return time
    else:
        return time.strftime('%Y-%m-%d-%H:%M:%S')

def get_dropdown_list(series):
    '''Given pandas series return list w/out None and timestamp/utime'''
    series = series.dropna()
    series = series.tolist()
    series.remove('timestamp')
    series.remove('utime')
    return series

def shift(time_list, time=False):
    if time:
        return [(time - time_list[0])*1e-6 for time in time_list]
    else:
        return [(time - time_list[0]) for time in time_list]

