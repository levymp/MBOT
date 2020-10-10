from pathlib import Path
import pandas as pd
from datetime import datetime
import analysis_utils as utils


def get_tables():
    try:
        prod_table = Path('/home/michaellevy/data/prod/mbot/mbot_table.pkl')
        backup_table = Path('/home/michaellevy/data/backup/mbot/mbot_table.pkl')
        prod_df = pd.read_pickle(prod_table)
        backup_df = pd.read_pickle(backup_table)
    except Exception:
        prod_df = utils.get_table('prod')
        backup_df = utils.get_table('backup')
    return prod_df, backup_df


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
    df_lookup = pd.DataFrame.from_dict(lookup, orient='index')

    # transpose columns
    df_lookup = df_lookup.transpose()
    
    # get weights lookup dictionary
    weights = dict(zip(df_lookup.keys(), weights))
    
    # get columns
    cols = list(df_lookup.keys())
    
    # sort columns
    cols.sort(key=lambda x: weights[x], reverse=True)
    
    # reorder dataframe
    df_lookup = df_lookup[cols]
    
    return df_lookup

def file_to_time(df, runId, dateobj=True):
    time = df.loc[runId]['PICKLE NAME'].split('.')[0]
    time = datetime.strptime(time, '%Y_%m_%d_%H:%M:%S')
    if dateobj == True:
        return time
    else:
        return time.strftime('%Y-%m-%d-%H:%M:%S')


