import requests

class ElogNotify:
    def __init__(self, url):
        self.url = url
    def log_message(self, logbook, **kwargs):
        print(kwargs)
        fields = dict(kwargs)
        fields.update(cmd='Submit')
        try:
            requests.post(self.url+logbook, files=fields, timeout=.00001)
        except:
            pass


if __name__ == '__main__':
    elog = ElogNotify('http://127.0.0.1:8080/')
    elog.log_message('Run',
        Author='RunControl',
        Subsystem='Run',
        Category='General',
        Subject='Terminated run %d'%209384092,
        Text='\n'.join([
            'Run terminated',
            'Run number:    %d'%209384092,
            'Run type:      %s'%'self.run.run_type',
            'Run crew:      %s'%'self.run.run_user',
            'Start message: %s'%'self.run.run_comment_start',
            'Stop message:  %s'%'self.run.run_comment_stop',
            ])
        )
