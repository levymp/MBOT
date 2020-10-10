from pathlib import Path
import pandas as pd
from datetime import datetime
import analysis_utils as utils


def get_tables():
    try:
        prod_table = Path('/home/michaellevy/data/prod/mbot/mbot_table.pkl')
        backup_table = Path('/home/michaellevy/data/backup/mbot/mbot_table.pkl')
        df_prod = pd.read_pickle(prod_table)
        df_backup = pd.read_pickle(backup_table)
    except Exception:
        df_prod = utils.get_table('prod')
        df_backup = utils.get_table('backup')

    # get custom indexes
    indexes = (_get_indexes(df_prod), _get_indexes(df_backup))

    return df_prod, df_backup, indexes


def _get_indexes(df):
    index_list = range(len(df))
    index_list = ["RUN " + str(i) for i in index_list]
    return index_list


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
