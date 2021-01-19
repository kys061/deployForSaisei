#!/usr/bin/python2.7
# Copyright (C) 2016 Saisei Networks Inc. All rights reserved.

# Periodic Reports will be sent to the configured email lists every day, week, and month.
# Works by instantiating a period_reporter and calling run() periodically.  If the previous
# day, week, or month has not been sent yet, send the needed reports.

import requests
import json
from datetime import date, time, datetime, timedelta
from saisei_smtp_client import sendemail

import logging
import logging.handlers
from traceback import format_exc
from time import sleep

LOG_FILENAME = "/var/log/stm_period_reporter.log"
ENABLE_DEBUG_LOGGING = True

REST_SERVER = 'localhost'
REST_PORT = 5000
REST_ROOT = 'rest/top/configurations/running'
REST_USER = '__internal__'
REST_PASSWORD = '__ssshhh__'

headers = {'content-type': 'application/json'}

def make_url(path) :
    return 'http://%s:%d%s' % (REST_SERVER, REST_PORT, path)

class period_reporter :

    def __init__(self, logger) :
        self.logger = logger
        self.session = None
        self.current_day = None
        self.current_week = None
        self.current_month = None
        self.smtp_server = None
        self.smtp_address = None
        self.smtp_password = None

    def _report_http_error(self, preamble, r) :
        """
        Deciphers an HTTP error payload so we can log something useful.
        """
        message = '?'
        if r.headers.get('content-type', '').lower().find('application/json') != -1 :
            try :
                payload = r.content.json()
                message = payload.get('message', '')
            except Exception as e :
                pass
        self.logger.error('%s, status: %d (%s)', str(preamble), r.status_code, message)

    def _get_date_thresholds(self) :
        """
        Calculates daily, weekly and monthly dates.
        """
        today = date.today()
        int_today = today.toordinal()
        self.current_day = str(date.fromordinal(int_today -1))

        monday = int_today - int_today % 7 + 1
        last_monday = monday - 7
        self.current_week = str(date.fromordinal(last_monday))
        if today.month == 1:
            report_year = today.year - 1
            report_month = 12
        else:
            report_year = today.year
            report_month = today.month - 1

        self.current_month = str(date(report_year, report_month, 1))

    def run(self) :
        success = True                          # Assume no *transient* errors
        self.logger.debug('Looking for scheduled reports')
        try :
            self.session = requests.Session()
            self.session.auth = (REST_USER, REST_PASSWORD)
            result = self.session.get(make_url('/rest/top/configurations/running/administrators/*/reports/?level=debug'))
            if result.status_code < requests.codes.bad :
                candidates = []
                payload = result.json()
                for report in payload['collection'] :
                    if report['email_list'] and (report['daily'] == 'T' or report['weekly'] == 'T' or report['monthly'] == 'T') :
                        candidates.append(report)
                if candidates :
                    result = self.session.get(make_url('/rest/top/configurations/running?select=smtp_address,smtp_password,smtp_server'))
                    if result.status_code < requests.codes.bad :
                        payload = result.json()
                        self.smtp_server = payload['collection'][0]['smtp_server']
                        self.smtp_address = payload['collection'][0]['smtp_address']
                        self.smtp_password = payload['collection'][0]['smtp_password']
                        if self.smtp_server and self.smtp_address :
                            # Here if we have some scheduled reports and a plausible SMTP configuration
                            self._get_date_thresholds()
                            for report in candidates :
                                success = self._process(report)
                        else :
                            self.logger.error('SMTP server and/or email client credentials are not configured')
                            success = False
                    else :
                        self._report_http_error('Error getting SMTP attributes', result)
                        success = False
                else :
                    self.session.close()
            else :
                self._report_http_error('Error getting report objects', result)
                success = False
        except Exception as e :
            self.logger.error('Report generation encountered an exception: %s', str(e))
            success = False                      # Assume it's a transient error
        return int(not success)                  # Caller expects 0/1 for success/failure

    def _process(self, report) :
        """
        Generate and email scheduled reports, should any be necessary at this time.
        """
        success = True
        if report['monthly'] == 'T' and self.current_month != report['last_monthly'] :
            success = success and self._make_document(report, 'monthly', self.current_month, 'last_monthly')
        if report['weekly'] == 'T' and self.current_week != report['last_weekly'] :
            success = success and self._make_document(report, 'weekly', self.current_week, 'last_weekly')
        if report['daily'] == 'T' and self.current_day != report['last_daily'] :
            success = success and self._make_document(report, 'daily', self.current_day, 'last_daily')
        return success

    def _make_document(self, report, period, for_date, update_field) :
        """
        Download and email one document instance.
        """
        success = True                          # False if an error is considered to be transient
        self.logger.debug('Generating %s instance of report \'%s\' for %s', period, report['name'], for_date)
        update = {'last_scheduled_result' : 'Success'}
        myreq = '/rest/top/public/report/%s/%s?report=%s' % (period, for_date, report['link']['href'])
        result = self.session.get(make_url(myreq))
        if result.status_code < requests.codes.bad :
            cd = result.headers.get('content-disposition', '').split(';')
            if cd and cd[0] == 'attachment' :
                filename = 'saisei_%s_report_%s_%s.pdf' % (period, report['name'], for_date) # Fallback
                for entry in cd :
                    pair = entry.strip().split('=', 1)
                    if pair[0] == 'filename' :
                        filename = pair[1]
                        break
                if len(result.content) :        # Email the report
                    error = sendemail(self.logger,
                                      to_addr_list = report['email_list'].split(','),
                                      subject = "Saisei Traffic Report: %s '%s' for %s" % (period, report['name'], for_date),
                                      string_attachments_dict = {filename: result.content},
                                      smtpserver = self.smtp_server,
                                      login = self.smtp_address,
                                      password = self.smtp_password)
                    if error :
                        self.logger.error("Failed to send %s email report '%s': %s" % (period, report['name'], str(error)))
                        update['last_scheduled_result'] = "Error sending %s report to '%s': %s" % (period, report['email_list'], str(error))
                        success = False
                    else :
                        update[update_field] = for_date
                else :
                      self.logger.error('%s report \'%s\' had empty content', period, report['name'])
                      update['last_scheduled_result'] = '%s report had empty content' % (period, )
            else :
                self.logger.error('Report \'%s\' does not have content-disposition attachment', report['name'])
                update['last_scheduled_result'] = '%s report \'%s\' did not have content-disposition attachment' % (period, report['name'])
        else :
            self._report_http_error('Error downloading %s report \'%s\'' % (period, report['name']), result)
            update['last_scheduled_result'] = 'HTTP Status %d' % result.status_code
        self.session.put(make_url(report['link']['href']),
                         data=json.dumps(update),
                         headers=headers)
        return success


if __name__ == "__main__" :
    logger = logging.getLogger('period_reporter')
    logger.setLevel(logging.DEBUG if ENABLE_DEBUG_LOGGING else logging.INFO)
    fh = logging.handlers.RotatingFileHandler(LOG_FILENAME, maxBytes=1000 * 1000 * 1, backupCount=3)
    fh.setFormatter(logging.Formatter('%(asctime)s %(levelname)s: %(message)s'))
    logger.addHandler(fh)
    logger.info('***** starting period_reporter *****')

    the_period_reporter = period_reporter(logger)
    never = date(1990, 1, 1)
    last_successful_day = never

    while True :
        try :
            session = requests.Session()
            session.auth = (REST_USER, REST_PASSWORD)
            resp = session.get(make_url('/rest/top/configurations/running?select=report_email_time'))
            session.close()
            if resp.status_code < requests.codes.bad :
                report_email_time = time(*[int(k) for k in resp.json()['collection'][0]['report_email_time'].split(':')])
                now_time = datetime.now().time().replace(microsecond=0)
                today = date.today()
                if last_successful_day < today and report_email_time <= now_time :
                    # Try to generate reports
                    if the_period_reporter.run() == 0 :
                        last_successful_day = today
                    # Wake-up again in 20 mins. If we just failed, we'll retry.
                    # If we succeeded, we'll immediately block until the next
                    # scheduled time. (This copes with the case that we're
                    # executing just before midnight, and generating the reports
                    # takes us until after midnight.
                    delay = 20 * 60.0
                else :
                    # It's not yet time to do today's run - wait until it is
                    delay = (datetime.combine(today, report_email_time) - datetime.combine(today, now_time)).seconds

            else :
                logger.debug('GET failed: %d %s', resp.status_code, resp.reason)
                delay = 60.0                    # Try again in a minute
            logger.debug('Last successful run was %s; waiting %s',
                         'never' if last_successful_day == never else str(last_successful_day),
                         str(timedelta(seconds=delay)))
            logger.debug('delay: %s', delay)
            sleep(delay)
        except Exception as e :
            logger.error('Unhandled exception: %s', str(e))
            logger.debug(format_exc())
