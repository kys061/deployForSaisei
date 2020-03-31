/*! saisei_report - v1.0.0 - 2018-03-27 */ 
'use strict';

var reportApp = angular.module('reportApp', [
    "ngRoute", 'base64', 'chart.js', 'angular-momentjs', 'angular-loading-bar',
    'angularjs-datetime-picker', 'angularjs-dropdown-multiselect', 'ui-notification'
])
    .config(['cfpLoadingBarProvider', function(cfpLoadingBarProvider) {
        cfpLoadingBarProvider.latencyThreshold = 400;
        cfpLoadingBarProvider.parentSelector = '#loading-bar-container';
        cfpLoadingBarProvider.spinnerTemplate = '<div id="loading-bar"><div id="loading-bar-spinner"><div class="spinner-icon"></div></div></div>';
    }])
    .config(function($routeProvider, $locationProvider, $momentProvider) {
        $momentProvider
            .asyncLoading(false)
            .scriptUrl('./lib/moment.min.js');
        $routeProvider
            .when('/report', {
                templateUrl: "templates/report.html",
                controller: "ReportCtrl"
            })
            .otherwise({
                redirectTo: '/'
            });
        $locationProvider.html5Mode(true);
    })
    .run(function($rootScope) {
        $rootScope.users_app_top1 = [];
    })
    .constant('_', window._);
reportApp.controller('MainCtrl', function MainCtrl($scope, $log, $route, $templateCache, $location, $window, $q, _,
                                                   SharedData, ReportMain, ReportMainMetaData) {
    var from;
    var until;
    var today = new $window.Sugar.Date(new Date());
    var complete_count;

    $scope.report_def = {1: 'int_report', 2: 'user_report', 3:'user_group_report'};
    $scope.select2model = [];
    $scope.select2data = [
        {
            id: 1,
            label: "인터페이스 트래픽"
        },
        {
            id: 2,
            label: "사용자 트래픽"
        },
        {
            id: 3,
            label: "사용자 그룹 트래픽"
        }
    ];
    $scope.select2settings = {};
    $scope.$watch('select2model', function(val){
        console.log(val);
    });
    $scope.currentState = true;
    $scope.currentDurationState = SharedData.currentDurationState;
    $scope.$watch('date_from', function(val) {
        from = val;
        console.log(val);
    });
    $scope.$watch('date_until', function(val) {
        until = val;
        console.log(val);
    });
    var getMetaData = function() {
        return $q(function(resolve, reject){
            var metaLink = new ReportMainMetaData();
            metaLink.q_metaLinkData().then(
                function(val){
                    $scope.hostname = val.hostname;
                    resolve({
                        hostname: $scope.hostname
                    });
                },
                function(val){
                    reject( Error("failure!!") );
                }
            )
        });
    };

    getMetaData().then(
        function(val){
            console.log(val);
            var hostname = val.hostname;
            ReportMain.getUserGroupSize(hostname).then(
                function (size) {
                console.log("group_size", size.data.size);
                $scope.group_size = size.data.size;
                ReportMain.getUserSize(hostname).then(
                    function (size) {
                        console.log("users_size", size.data.size);
                        $scope.users_size = size.data.size
                    },
                    function(val){
                        notie.alert({
                            type: 'error',
                            text: '사용자 그룹 정보를 가지고 올 수 없습니다!!!'
                        })
                    }
                );
                },
                function(val){
                    notie.alert({
                        type: 'error',
                        text: '사용자 그룹 정보를 가지고 올 수 없습니다!!!'
                    })
                }
            );
        },
        function(val){
            console.log(val);
        }
    );

    $scope.sendDate = function() {
        var duration = $window.Sugar.Date.range(from, until).every('days').length;
        console.log(duration);
        console.log($scope.select2model);

        $scope.report_type = [];
        for (var i = 0; i < $scope.select2model.length; i++) {
            $scope.report_type.push({name : $scope.report_def[$scope.select2model[i]['id']], status: true});
        }

        var _until = new $window.Sugar.Date(until);
        var _from = new $window.Sugar.Date(from);
        console.log("from : until -> " + _from.raw + ':' + _until.raw);
        if (from === undefined || until === undefined) {
            notie.alert({
                type: 'error',
                text: '리포트 기간을 넣어주세요!!!'
            })
        } else if (duration > 31) {
            notie.alert({
                type: 'error',
                text: '리포트 기간은 최대 한달까지 가능합니다!!'
            })
        } else if (_until.isFuture().raw) {
            notie.alert({
                type: 'error',
                text: '리포트 종료 시점은 현재보다 미래로 설정할 수 없습니다!!'
            })
        } else if (_from.isFuture().raw) {
            notie.alert({
                type: 'error',
                text: '리포트 시작 시점은 현재보다 미래로 설정할 수 없습니다!!'
            });
        } else {
            if($scope.select2model.length > 0) {
                console.log($scope.group_size);
                var count_group = 0;
                var count_users = 0;
                _($scope.select2model).each(function(elem, index){
                    if (elem.id === 3){
                        count_group += 1;
                    }
                    if (elem.id === 2){
                        count_users += 1;
                    }
                });
                if ($scope.group_size > 0 && $scope.users_size > 0) {
                    console.log('count_group', count_group);
                    complete_count=2+4+12+3+$scope.group_size;
                    $scope.currentState = false;
                    $scope.currentDurationState = false;
                    SharedData.setFrom(from);
                    SharedData.setUntil(until);
                    SharedData.setSelect2model($scope.select2model);
                    SharedData.setReportType($scope.report_type);
                    console.log($scope.report_type);
                    $location.path('/report');
                }
                else if ($scope.group_size > 0 && $scope.users_size <= 0){
                    if (count_users > 0) {
                        notie.alert({
                            type: 'error',
                            text: '사이세이 내에 사용자가 존재 하지 않습니다. 사용자 트래픽은 해제해 주십시요!!!'
                        })
                    } else{
                        $scope.currentState = false;
                        $scope.currentDurationState = false;
                        SharedData.setFrom(from);
                        SharedData.setUntil(until);
                        SharedData.setSelect2model($scope.select2model);
                        SharedData.setReportType($scope.report_type);
                        console.log($scope.report_type);
                        $location.path('/report');
                    }
                }
                else if ($scope.group_size <= 0 && $scope.users_size > 0){
                    if (count_group > 0) {
                        notie.alert({
                            type: 'error',
                            text: '사이세이 내에 사용자 그룹이 존재 하지 않습니다. 사용자 그룹은 해제해 주십시요!!!'
                        })
                    } else{
                        $scope.currentState = false;
                        $scope.currentDurationState = false;
                        SharedData.setFrom(from);
                        SharedData.setUntil(until);
                        SharedData.setSelect2model($scope.select2model);
                        SharedData.setReportType($scope.report_type);
                        console.log($scope.report_type);
                        $location.path('/report');
                    }
                }
                else {
                    console.log('count_group', count_group);
                    if (count_group > 0) {
                        notie.alert({
                            type: 'error',
                            text: '사이세이 내에 사용자 그룹이 존재 하지 않습니다. 사용자 그룹 트래픽은 해제해 주십시요!!!'
                        })
                    }
                    else if(count_users > 0){
                        notie.alert({
                            type: 'error',
                            text: '사이세이 내에 사용자가 존재 하지 않습니다. 사용자 트래픽은 해제해 주십시요!!!'
                        })
                    }
                    else{
                        $scope.currentState = false;
                        $scope.currentDurationState = false;
                        SharedData.setFrom(from);
                        SharedData.setUntil(until);
                        SharedData.setSelect2model($scope.select2model);
                        SharedData.setReportType($scope.report_type);
                        console.log($scope.report_type);
                        $location.path('/report');
                    }
                }
            }
            else{
                notie.alert({
                    type: 'error',
                    text: '최소 하나의 리포트를 선택해주세요!'
                })
            }
        }
    };
});
reportApp.controller('ReportCtrl', function ReportCtrl(
    $rootScope, $scope, $log, _, ReportData, SharedData, UserAppData, $location, $route, $window, cfpLoadingBar,
    $q, $timeout, ReportInterfaceData, ReportUserData, ReportMetaData, ReportIntName, ReportUserGroupData,
    Notification) {

    $scope.$on('$routeChangeStart', function(scope, next, current) {
        SharedData.setCurrentState(true);
        console.log("change back");
        $location.path('/');
        $window.location.href = '/report/';
    });
    var req_count = 2+4+12+3+22;
    $scope.complete_check_count = req_count; // 나중에 계산 수식 필요~!!
    $scope.loaded_count = 0;
    $scope.started_count = 0;
    $scope.created_time = (new Date()).toLocaleString('ko-KR',{hour12: false}).replace('시 ', ':').replace('분 ', ':').replace('초','');
    $rootScope.$on('cfpLoadingBar:loading', function() {
        $scope.started_count += 1;
    });

    $rootScope.$on('cfpLoadingBar:loaded', function() {
        $scope.loaded_count += 1;
        console.log("loaded_count : " + $scope.loaded_count);
    });

    $rootScope.$on('cfpLoadingBar:completed', function() {
        console.log('cfpLoadingBar:completed!!!!!!!!!!!!!!!!!!!');
    });

    var from = SharedData.getFrom();
    var until = SharedData.getUntil();
    var select2model = SharedData.getSelect2model();
    var report_type = SharedData.getReportType();
    console.log("from : until -> " + from + ':' + until);
    $scope.grpState = [
        {
            "name": "int_report",
            "state": false
        },
        {
            "name": "user_report",
            "state": false
        },
        {
            "name": "user_group_report",
            "state": false
        }
    ];

    $scope.segState =[
        {
            name: "seg1",
            state: false
        },
        {
            name: "seg2",
            state: false
        }
    ];

    for (var k = 0; k < report_type.length; k++) {
        if (report_type[k].status) {
            for (var j = 0; j < $scope.grpState.length; j++) {
                if ($scope.grpState[j].name === report_type[k].name) {
                    $scope.grpState[j].state = true;
                }
            }
        }
    }

    $scope.getGraphState = function(arr, name) {
        for (var i = 0; i < arr.length; i++) {
            if (arr[i].name === name) {
                return arr[i].state === true;
            }
        }
    };
    console.log(from + " - " + until);

    $scope.getSegState = function(arr, name) {
        for (var i = 0; i < arr.length; i++) {
            if (arr[i].name === name) {
                return arr[i].state === true;
            }
        }
    };
    var getMetaData = function() {
        return $q(function(resolve, reject){
            var metaLink = new ReportMetaData();
            metaLink.q_metaLinkData().then(
                function(val) {
                    console.log(val);
                    $scope.intLink = val.int_link;
                    $scope.hostname = val.hostname;
                    var IntName = new ReportIntName();
                    IntName.q_intName($scope.hostname).then(
                        function (val) {
                            console.log(val);
                            resolve({
                                hostname: $scope.hostname,
                                int_ext_name: val.int_ext_name,
                                int_int_name: val.int_int_name,
                                use_span: false
                            });
                        },
                        function (val) {
                            console.log(val);
                            reject(Error("failure!!"));
                        }
                    );

                        },
                        function(val){
                            console.log(val);
                            reject( Error("failure!!") );
                        }
                    );

        });
    };
    getMetaData().then(
        function(val){
            var hostname = val.hostname;
            if (val.int_ext_name.length > 1){
                var first_seg_int_ext_name = val.int_ext_name[0];
                var second_seg_int_ext_name = val.int_ext_name[1];
                var first_seg_int_int_name = val.int_int_name[0];
                var second_seg_int_int_name = val.int_int_name[1];
                console.log('인터페이스 이름: ', val.int_ext_name, val.int_int_name);
                _.each($scope.segState, function(state){
                    state.state = true
                });
                var firstSegIntGrpDataset = new ReportInterfaceData();
                firstSegIntGrpDataset.q_intData(hostname, first_seg_int_ext_name, first_seg_int_int_name, from, until, duration, $scope.grpState[0].state).then(
                    function(val){
                        $scope.data = val.data;
                        $scope.labels = val.labels;
                        $scope.series = val.series;
                        $scope.colors = val.colors;
                        $scope.options = val.options;
                        $scope.datasetOverride = val.datasetOverride;
                        $scope.int_data = val.int_data;
                        $scope.int_name = val.int_name;
                        $scope.complete_first_seg_count = val.complete_count;
                    },
                    function(val){
                        console.log(val);
                    }
                );
                var secondSegIntGrpDataset = new ReportInterfaceData();
                secondSegIntGrpDataset.q_intData(hostname, second_seg_int_ext_name, second_seg_int_int_name, from, until, duration, $scope.grpState[0].state).then(
                    function(val){
                        $scope.second_seg_data = val.data;
                        $scope.second_seg_labels = val.labels;
                        $scope.second_seg_series = val.series;
                        $scope.second_seg_colors = val.colors;
                        $scope.second_seg_options = val.options;
                        $scope.second_seg_datasetOverride = val.datasetOverride;
                        $scope.second_seg_int_data = val.int_data;
                        $scope.second_seg_int_name = val.int_name;
                        $scope.complete_second_seg_count = val.complete_count;
                    },
                    function(val){
                        console.log(val);
                    }
                );
            }else{
                _.each($scope.segState, function(state, state_index){
                    if(state_index === 0) state.state = true;
                });
                var first_seg_int_ext_name = val.int_ext_name[0];
                var first_seg_int_int_name = val.int_int_name[0];
                var firstSegIntGrpDataset = new ReportInterfaceData();

                firstSegIntGrpDataset.q_intData(hostname, first_seg_int_ext_name, first_seg_int_int_name, from, until, duration, $scope.grpState[0].state).then(
                    function(val){
                        $scope.data = val.data;
                        $scope.labels = val.labels;
                        $scope.series = val.series;
                        $scope.colors = val.colors;
                        $scope.options = val.options;
                        $scope.datasetOverride = val.datasetOverride;
                        $scope.int_data = val.int_data;
                        $scope.int_name = val.int_name;
                        $scope.complete_first_seg_count = val.complete_count;
                    },
                    function(val){
                        console.log(val);
                    }
                );
            }
            var userGrpDataset = new ReportUserData();
            userGrpDataset.q_userData(hostname, from, until, duration, $scope.grpState[1].state).then(
                function(val){
                    console.log(val.user._users_data);
                    $scope._users_tb_data = val.user._users_tb_data;
                    $scope._users_data = val.user._users_data;
                    $scope._users_flow_disc_data = val.user._users_flow_disc_data;
                    $scope._users_label = val.user._users_label;
                    $scope._users_series = val.user._users_series;
                    $scope._users_flow_disc_series = val.user._users_flow_disc_series;
                    $scope._users_option = val.user._users_option;
                    $scope._users_flow_disc_option = val.user._users_flow_disc_option;
                    $scope._users_datasetOverride = val.user._users_datasetOverride;
                    $scope.colors = val.user.colors;
                    $scope._users_app = val.user_app._users_app; // for table
                    $scope._users_app_data = val.user_app._users_app_data;
                    $scope._users_app_label = val.user_app._users_app_label;
                    $scope._users_app_series = val.user_app._users_app_series;
                    $scope._users_app_option = val.user_app._users_app_option;
                    console.log('user_count: ', val.complete_count);
                    $scope.complete_user_count = val.complete_count;
                },
                function(val){
                    console.log(val);
                }
            );
            $scope._user_group_size = 0;

            var userGroupGrpData = new ReportUserGroupData();
            userGroupGrpData.q_userGroupData(hostname, from, until, duration, $scope.grpState[2].state).then(
                function(val) {
                    console.log(' 3. 유저 그룹 트래픽 그래프');
                    console.log(val);
                    $scope._user_group_label = val.user_group._user_group_label;
                    $scope._user_group_data = val.user_group._user_group_data;
                    $scope._user_group_series = val.user_group._user_group_series;
                    $scope._user_group_option = val.user_group._user_group_option;
                    $scope._user_group_colors = val.user_group._user_group_colors;
                    $scope._user_group_size = val.user_group._user_group_size;
                    $scope._user_group_tb_data = val.user_group._user_group_tb_data;
                    $scope._user_group_size = val.user_group._user_group_size;
                    $scope._user_in_group_tb = val.user_in_group._user_in_group_tb; // for table
                    $scope._user_in_group_tr_data = val.user_in_group._user_in_group_tr_data;
                    $scope._user_in_group_label = val.user_in_group._user_in_group_label;
                    $scope._user_in_group_series = val.user_in_group._user_in_group_series;
                    $scope._user_in_group_option = val.user_in_group._user_in_group_option;
                    $scope.complete_usergroup_count = val.complete_count;
                },
                function(val) {
                    console.log(val);
                })
        },
        function(val){
            console.log(val);
        }
    );
    $scope.export_print =  function(){
        $window.print();
    };
    var size = {};
    size.header_page = {};
    size.first_page = {};
    size.second_page = {};
    size.third_page = {};
    size.fourth_page = {};
    size.fifth_page = {};
    size.fifth_page_grp = {};
    size.fifth_page_tb = {};
    size.fifth_page_tb_group={};
    size.fifth_page_tb_set={};
    size.last_page = {};
    var ratio = 2.2, second_ratio = 3, fourth_ratio = 3, fifth_ratio = 3
    var _from = new Date(from);
    var _until = new Date(until);
    $scope.from = _from.toLocaleString();
    $scope.until = _until.toLocaleString();

    var duration = $window.Sugar.Date.range(from, until).every('days').length;
    $scope.back = function() {
        $window.location.reload();
    };
    $scope.export_xls = function() {
        if($scope.grpState[0].state && $scope.grpState[1].state) {
            var data1 = alasql('SELECT * FROM HTML("#table1",{headers:true})');
            var data2 = alasql('SELECT * FROM HTML("#table2",{headers:true})');
            var data3 = alasql('SELECT * FROM HTML("#table3",{headers:true})');
            var int_file = 'SELECT * INTO CSV("interface-'+ $scope.from+"~"+$scope.until + '.csv",{headers:true, separator:","}) FROM ?';
            alasql(int_file, [data1]);
            alasql('SELECT * INTO CSV("user_traffic.csv",{headers:true, separator:","}) FROM ?', [data2]);
            alasql('SELECT * INTO CSV("user_app_traffic.csv",{headers:true, separator:","}) FROM ?', [data3]);
            notie.alert({
                type: 'error',
                text: 'csv파일이 생성되었습니다!'
            });
        } else {
            if($scope.grpState[0].state){
                var data1 = alasql('SELECT * FROM HTML("#table1",{headers:true})');
                alasql('SELECT * INTO CSV("interface.csv",{headers:true, separator:","}) FROM ?', [data1]);
                notie.alert({
                    type: 'error',
                    text: 'csv파일이 생성되었습니다!'
                });
            } else if($scope.grpState[1].state){
                var data2 = alasql('SELECT * FROM HTML("#table2",{headers:true})');
                var data3 = alasql('SELECT * FROM HTML("#table3",{headers:true})');
                alasql('SELECT * INTO CSV("user_traffic.csv",{headers:true, separator:","}) FROM ?', [data2]);
                alasql('SELECT * INTO CSV("user_app_traffic.csv",{headers:true, separator:","}) FROM ?', [data3]);
                notie.alert({
                    type: 'error',
                    text: 'csv파일이 생성되었습니다!'
                });
            } else {
                notie.alert({
                    type: 'error',
                    text: '출력할 데이터가 존재하지 않습니다.'
                });
            }
        }
    };
});
reportApp.service('ReportAuth', function($base64) {
    var Auth = function (start) {
        var self = this;
        this.addId = function(id){
            start = start + id;
            return self;
        };
        this.addPasswd = function(pass){
            start = start + ":" + pass;
            return self;
        };
        this.getAuth = function(){
            return {
                "Authorization": "Basic " + $base64.encode(start)
            };
        };
    };
    return Auth;
});
reportApp.service('ReportConfig', function($q) {
    var Config = function() {
        var self = this;
        var result;
        this.getConfig = function() {
            $.getJSON("./config/report-config.json", function (d) {
                result = d.config;
            });
            return result;
        };
        this.q_configData = function() {
            var deferred = $q.defer();
            $.getJSON("./config/report-config.json", function (d) {
                deferred.resolve(d.config);
            });
            return deferred.promise;
        }
    };

    return Config;
});
reportApp.service('ReportFrom', function() {
    var From = function (start) {
        var self = this;
        this.setFrom = function(_from){
            var from = new Date(_from);
            var _from_yy = moment(from.toUTCString()).utc().format('YYYY');
            var _from_mm = moment(from.toUTCString()).utc().format('MM');
            var _from_dd = moment(from.toUTCString()).utc().format('DD');
            var _from_hh = moment(from.toUTCString()).utc().format('HH');
            var _from_min = moment(from.toUTCString()).utc().format('mm');
            var _from_sec = moment(from.toUTCString()).utc().format('ss');
            start = start+_from_hh+":"+_from_min+":"+_from_sec+"_"+_from_yy+_from_mm+_from_dd;
            return self;
        };
        this.getFrom = function(){
            return start;
        };
    };
    return From;
});
reportApp.service('ReportInterfaceData', function($window, $q, ReportData) {
    var InterfaceRate = function() {
        var self = this;
        function makeRcvData(_history_length_rcv_rate, _history_rcv, from_date, _from_date, duration){
            var label = [];
            var data_rcv_rate = [];
            var int_date = [];
            var int_cmp_date = [];
            var int_rcv_avg = [];
            var int_rcv_max_data = [];
            var int_rcv_duration_max_data = [];
            var int_rcv_duration_max_date = [];
            var rcv_tot = [];
            var rcv_len = [];
            int_date.push(from_date.format("%F"));
            int_cmp_date.push(_from_date.format("%m-%d"));
            for (var j = 0; j < duration - 1; j++) {
                int_date.push(from_date.addDays(1).format("%F").raw);
                int_cmp_date.push(_from_date.addDays(1).format("%m-%d"));
            }
            for (var i = 0; i < _history_length_rcv_rate; i++) {
                if (i % 20 === 0) {
                    var t = new Date(_history_rcv[i][0]);
                    label.push(t.toLocaleString());
                    data_rcv_rate.push((_history_rcv[i][1] * 0.001).toFixed(3));
                }
            }
            for (var j = 0; j < duration; j++) {
                rcv_tot.push(0);
                rcv_len.push(0);
                int_rcv_duration_max_data.push([]);
                int_rcv_duration_max_date.push([]);
            }
            for (var j = 0; j < duration; j++) {
                for (var i = 0; i < _history_length_rcv_rate; i++) {
                    if (int_cmp_date[j].raw === moment(_history_rcv[i][0]).format('MM-DD')) {
                        rcv_tot[j] += _history_rcv[i][1]*0.001;
                        rcv_len[j] += 1;
                        int_rcv_duration_max_data[j].push(_history_rcv[i][1]*0.001);
                        int_rcv_duration_max_date[j].push(_history_rcv[i][0]);
                    }else {
                        rcv_tot[j] += 0;
                        rcv_len[j] += 1;
                        int_rcv_duration_max_data[j].push(0);
                        int_rcv_duration_max_date[j].push(NaN);
                    }
                }
            }
            for (var j = 0; j < duration; j++) {
                int_rcv_avg.push(rcv_tot[j] / rcv_len[j]);
                console.log("RCV");
                console.log(j, rcv_tot[j],rcv_len[j])
            }
            for (var j = 0; j < duration; j++) {
                if (Math.max.apply(null, int_rcv_duration_max_data[j]) === 0) {
                    int_rcv_max_data.push({
                        rcv_max_rate: 0,
                        rcv_max_date: NaN
                    });
                } else {
                    int_rcv_max_data.push({
                        rcv_max_rate: Math.max.apply(null, int_rcv_duration_max_data[j]),
                        rcv_max_date: int_rcv_duration_max_date[j][int_rcv_duration_max_data[j].indexOf(Math.max.apply(null, int_rcv_duration_max_data[j]))]
                    });
                }
                console.log("RCV MAX");
            }
            console.log("int_rcv_max_data", int_rcv_max_data);
            return {
                label: label,
                int_date: int_date,
                int_cmp_date: int_cmp_date,
                data_rcv_rate: data_rcv_rate,
                int_rcv_avg: int_rcv_avg,
                int_rcv_max_data: int_rcv_max_data
            }
        }
        function makeTrsData(_history_length_trs_rate, _history_trs, int_cmp_date, duration){
            var data_trs_rate = [];
            var int_trs_avg = [];
            var int_trs_max_data = [];
            var int_trs_duration_max_data = [];
            var int_trs_duration_max_date = [];
            var trs_tot = [];
            var trs_len = [];
            for (var i = 0; i < _history_length_trs_rate; i++) {
                if (i % 20 === 0) {
                    data_trs_rate.push((_history_trs[i][1] * 0.001).toFixed(3));
                }
            }
            for (var j = 0; j < duration; j++) {
                trs_tot.push(0);
                trs_len.push(0);
                int_trs_duration_max_data.push([]);
                int_trs_duration_max_date.push([]);
            }
            for (var j = 0; j < duration; j++) {
                for (var i = 0; i < _history_length_trs_rate; i++) {
                    if (int_cmp_date[j].raw === moment(_history_trs[i][0]).format('MM-DD')) {
                        trs_tot[j] += _history_trs[i][1]*0.001;
                        trs_len[j] += 1;
                        int_trs_duration_max_data[j].push(_history_trs[i][1]*0.001);
                        int_trs_duration_max_date[j].push(_history_trs[i][0]);
                        console.log(int_cmp_date[j].raw);
                    }else {
                        trs_tot[j] += 0;
                        trs_len[j] += 1;
                        int_trs_duration_max_data[j].push(0);
                        int_trs_duration_max_date[j].push(NaN);
                    }
                }
            }
            for (var j = 0; j < duration; j++) {
                int_trs_avg.push(trs_tot[j] / trs_len[j]);
                console.log("TRS");
                console.log(j, trs_tot[j],trs_len[j])
            }
            for (var j = 0; j < duration; j++) {
                console.log("trs_max_date_index", int_trs_duration_max_data[j].indexOf(Math.max.apply(null, int_trs_duration_max_data[j])), Math.max.apply(null, int_trs_duration_max_data[j]));
                int_trs_max_data.push({
                    trs_max_rate: Math.max.apply(null, int_trs_duration_max_data[j]),
                    trs_max_date: int_trs_duration_max_date[j][int_trs_duration_max_data[j].indexOf(Math.max.apply(null, int_trs_duration_max_data[j]))]
                });
                console.log("TRS MAX");
            }
            return {
                data_trs_rate: data_trs_rate,
                int_trs_avg: int_trs_avg,
                int_trs_max_data: int_trs_max_data
            }
        }
        function makeReportInterfaceData(int_date, data_rcv_rate, int_rcv_avg, int_rcv_max_data, data_trs_rate, int_trs_avg, int_trs_max_data){
            var int_data = [];
            for (var k = 0; k < int_date.length; k++) {
                console.log(int_rcv_avg[k]);
                int_data.push({
                    date: int_date[k],
                    rcv_avg: (!isNaN(int_rcv_avg[k])) ? int_rcv_avg[k].toFixed(3):0,
                    trs_avg: (!isNaN(int_trs_avg[k])) ? int_trs_avg[k].toFixed(3):0,
                    rcv_max: (!isNaN(int_rcv_max_data[k].rcv_max_rate)) ? int_rcv_max_data[k].rcv_max_rate.toFixed(3):0,
                    rcv_max_date: (!isNaN(int_rcv_max_data[k].rcv_max_date  && int_rcv_max_data[k].rcv_max_rate !== 0 )) ? (new Date(int_rcv_max_data[k].rcv_max_date)).toLocaleString():'none',
                    trs_max: (!isNaN(int_trs_max_data[k].trs_max_rate)) ? int_trs_max_data[k].trs_max_rate.toFixed(3):0,
                    trs_max_date: (!isNaN(int_trs_max_data[k].trs_max_date) && int_trs_max_data[k].trs_max_rate !== 0 ) ? (new Date(int_trs_max_data[k].trs_max_date)).toLocaleString():'none'
                });
            }
            console.log('int_data');
            console.log(int_data);
            var intGrpData = [
                data_rcv_rate,
                data_trs_rate
            ];
            console.log(data_rcv_rate, data_trs_rate);
            var int_rcv_max = Math.max.apply(null, data_rcv_rate);
            var int_trs_max = Math.max.apply(null, data_trs_rate);
            var int_max = Math.max.apply(null, [int_rcv_max, int_trs_max]);

            console.log(int_max);

            var option_max = Math.round(int_max);
            var options = {
                scales: {
                    yAxes: [{
                        id: 'y-axis-1',
                        type: 'linear',
                        display: true,
                        position: 'left',
                        scaleLabel: {
                            display: true,
                            fontSize: 14,
                            labelString: '수신(Mbit/s)',
                            fontStyle: "bold"
                        },
                        ticks: {
                            max: option_max,
                            min: 0,
                            beginAtZero: true,
                            fontSize: 12,
                            fontStyle: "bold"
                        }
                    },
                        {
                            id: 'y-axis-2',
                            type: 'linear',
                            display: true,
                            position: 'right',
                            scaleLabel: {
                                display: true,
                                fontSize: 14,
                                labelString: '송신(Mbit/s)',
                                fontStyle: "bold"
                            },
                            ticks: {
                                max: option_max,
                                min: 0,
                                beginAtZero: true,
                                fontSize: 12,
                                fontStyle: "bold"
                            }
                        }
                    ],
                    xAxes: [{
                        ticks: {
                            fontSize: 12,
                            fontStyle: "bold"
                        },
                        scaleLabel: {
                            display: true,
                            fontSize: 14,
                            labelString: '시간',
                            fontStyle: "bold"
                        }
                    }]
                }
            };

            return {
                data: intGrpData,
                options: options,
                int_data: int_data
            }
        }
        this.q_intData = function(hostname, int_ext_name, int_int_name, from, until, duration, isset) {
            var deferred = $q.defer();
            var from = from;
            var until = until;
            var duration = duration;
            var from_date = new $window.Sugar.Date(from);
            var _from_date = new $window.Sugar.Date(from);
            var complete_count = 0;
            if (isset) {
                ReportData.getIntRcvData(hostname, int_ext_name).then(function (data) {
                    complete_count += 1;
                    var _history_length_rcv_rate = data['data']['collection'][0]['_history_length_receive_rate'];
                    var _history_rcv = data['data']['collection'][0]['_history_receive_rate'];
                    var int_name = data['data']['collection'][0]['name'];
                    var r__rcvData = makeRcvData(_history_length_rcv_rate, _history_rcv, from_date, _from_date, duration);
                    var labels = r__rcvData.label;
                    var int_date = r__rcvData.int_date;
                    var int_cmp_date = r__rcvData.int_cmp_date;
                    var data_rcv_rate = r__rcvData.data_rcv_rate;
                    var int_rcv_avg = r__rcvData.int_rcv_avg;
                    var int_rcv_max_data = r__rcvData.int_rcv_max_data;
                    var series = ['수신(단위:Mbit/s)', '송신(단위:Mbit/s)'];
                    var colors = ['#ff6384', '#45b7cd', '#ffe200'];
                    var datasetOverride = [{
                        yAxisID: 'y-axis-1'
                    }, {
                        yAxisID: 'y-axis-2'
                    }];

                    ReportData.getIntRcvData(hostname, int_int_name).then(function (data) {
                        complete_count += 1;
                        var _history_length_trs_rate = data['data']['collection'][0]['_history_length_receive_rate'];
                        var _history_trs = data['data']['collection'][0]['_history_receive_rate'];
                        var r__trsData = makeTrsData(_history_length_trs_rate, _history_trs, int_cmp_date, duration);
                        var data_trs_rate = r__trsData.data_trs_rate;
                        var int_trs_avg = r__trsData.int_trs_avg;
                        var int_trs_max_data = r__trsData.int_trs_max_data;




                        var r__reportInterfaceData = makeReportInterfaceData(int_date, data_rcv_rate, int_rcv_avg, int_rcv_max_data,
                            data_trs_rate, int_trs_avg, int_trs_max_data);


                        deferred.resolve({
                            data: r__reportInterfaceData.data,
                            labels: labels,
                            series: series,
                            colors: colors,
                            options: r__reportInterfaceData.options,
                            datasetOverride: datasetOverride,
                            int_data: r__reportInterfaceData.int_data,
                            int_name: int_name,
                            complete_count: complete_count
                        });
                    });
                });
            }
            return deferred.promise;
        };
    };
    return InterfaceRate;
});
reportApp.service('ReportIntName', function($window, $q, ReportData) {
    var intName = function() {
        var self = this;
        this.q_intName = function(hostname) {
            var deferred = $q.defer();
            ReportData.getInterfaceName(hostname).then(function (data) {
                var collection = data.data.collection;
                var int_ext_name = [];
                var int_int_name = [];
                for (var i = 0; i < collection.length; i++){
                    int_ext_name.push(collection[i].name);
                    int_int_name.push(collection[i].peer.link.name);
                }
                deferred.resolve({
                    int_ext_name: int_ext_name,
                    int_int_name: int_int_name
                });
            });
            return deferred.promise;
        };
    };
    return intName;
});
reportApp.service('ReportMainMetaData', function($window, $q, ReportMain) {
    var metaData = function() {
        var self = this;
        this.q_metaLinkData = function() {
            var deferred = $q.defer();
            ReportMain.getMetaLink().then(function (data) {
                var metadata = data;
                var intLink = metadata.data.collection[0]['interfaces'].link.href;
                var hostname = metadata.data.collection[0]['system_name'];

                deferred.resolve({
                    int_link: intLink,
                    hostname: hostname
                });
            });
            return deferred.promise;
        };
    };
    return metaData;
});
reportApp.service('ReportMetaData', function($window, $q, ReportData) {
    var metaData = function() {
        var self = this;
        this.q_metaLinkData = function() {
            var deferred = $q.defer();
            ReportData.getMetaLink().then(function (data) {
                var metadata = data;
                var intLink = metadata.data.collection[0]['interfaces'].link.href;
                var hostname = metadata.data.collection[0]['system_name'];

                deferred.resolve({
                    int_link: intLink,
                    hostname: hostname
                });
            });
            return deferred.promise;
        };
    };
    return metaData;
});
reportApp.service('ReportQstring', function() {
    var Qstring = function (start) {
        var self = this;
        this.addSelect = function(attr){
            start = start + attr;
            return self;
        };
        this.addOperation = function(operation){
            start = start + operation;
            return self;
        };
        this.addOrder = function(order){
            start = start + order;
            return self;
        };
        this.addHistPoint = function(val){
            start = start + val;
            return self;
        };
        this.addLimit = function(limit){
            start = start + limit;
            return self;
        };
        this.addWith = function(_with){
            start = start + _with;
            return self;
        };
        this.addFrom = function(from){
            start = start + from;
            return self;
        };
        this.addUntil = function(until){
            start = start + until;
            return self;
        };

        this.getQstring = function(){
            return start;
        };
    };

    return Qstring;
});
reportApp.service('ReportUntil', function() {
    var Until = function (start) {
        var self = this;
        this.setUntil = function(_until){
            var until = new Date(_until);
            var _until_yy = moment(until.toUTCString()).utc().format('YYYY');
            var _until_mm = moment(until.toUTCString()).utc().format('MM');
            var _until_dd = moment(until.toUTCString()).utc().format('DD');
            var _until_hh = moment(until.toUTCString()).utc().format('HH');
            var _until_min = moment(until.toUTCString()).utc().format('mm');
            var _until_sec = moment(until.toUTCString()).utc().format('ss');
            start = start+_until_hh+":"+_until_min+":"+_until_sec+"_"+_until_yy+_until_mm+_until_dd;
            return self;
        };
        this.getUntil = function(){
            return start;
        };
    };
    return Until;
});
reportApp.service('ReportUrl', function() {
    var Urls = function (start) {
        var self = this;
        this.addDefault = function(ip, port, path){
            start = start + ip + port + path;
            return self;
        };
        this.addSection = function(section){
            start = start + section;
            return self;
        };
        this.addQstring = function(qstring){
            start = start + qstring;
            return self;
        };
        this.getUrls = function(){
            return start;
        };
    };

    return Urls;
});
reportApp.service('ReportUserData', function($window, $q, ReportData, UserAppData, _) {
    var UserData = function() {
        var self = this;
        this.q_userData = function(hostname, from, until, duration, isset) {
            var deferred = $q.defer();
            var from = from;
            var until = until;
            var duration = duration;
            var complete_count = 0;
            if (isset) {
                ReportData.getUserActiveFlows(hostname).then(function (data) {
                    complete_count += 1;
                    console.log(data);
                    var _history_users_active_flows_data = data['data']['collection'];
                    var _history_users_active_flows = data['data']['collection'][0]['_history_active_flows'];
                    var _history_users_active_flows_length = data['data']['collection'][0]['_history_length_active_flows'];
                    var users_act_flow_data = [];
                    var users_act_flow_time = [];
                    var arr_users_act_flow_data = [];
                    var arr_users_act_flow_time = [];
                    var users_act_flow_max_data = [];
                    _.each(_history_users_active_flows_data, function(collection) {
                        users_act_flow_max_data.push(_.max(collection['_history_active_flows'], function(history_active_flows){
                            return history_active_flows[1];
                        }));
                    });
                    _.each(users_act_flow_max_data, function(elem, index, data){
                        var t = new Date(elem[0]);
                        elem.push(t.toLocaleString());
                    });

                    ReportData.getUserData(hostname).then(function (data) {
                        complete_count += 1;
                        var _users_label = [];
                        var _users_from = [];
                        var _users_until = [];
                        var _users_series = ['다운로드 사용량(Mbit/s)', '업로드 사용량(Mbit/s)'];
                        var _users_flow_disc_series = ['플로우 사용량(/s)', '제어량(/s)'];
                        var _users_total = [];
                        var _users_download = [];
                        var _users_upload = [];
                        var _users_active_flows = [];
                        var _users_packet_disc_rate = [];
                        var _users_packet_disc = [];
                        var _users_tb_data = [];
                        var _users_data = [];
                        var _users_flow_disc_data = [];
                        var _users_app = [];
                        var _users_app_top1 = [];
                        var _users_app_top2 = [];
                        var _users_app_top3 = [];
                        var _users_app_data = [];
                        var _users_appName_top1 = [];
                        var _users_appName_top2 = [];
                        var _users_appName_top3 = [];
                        var _users_app_label = [];
                        var _users_app_series = [];
                        var _users_app_option = [];

                        var colors = ['#ff6384', '#45b7cd', '#ffe200'];
                        var _users = data['data']['collection'];
                        // for saisei-7.3.1-11114
                        var from = new Date(data['data']['from']);
                        from.setHours(from.getHours() + 9);
                        var _from = from.toLocaleString();
                        var until = new Date(data['data']['until']);
                        until.setHours(until.getHours() + 9);
                        var _until = until.toLocaleString();
                        for (var i = 0; i < _users.length; i++) {
                            _users_label.push(_users[i]['name']);
                            // var user_from = new Date(_users[i]['from']);
                            // user_from.setHours(user_from.getHours() + 9);
                            var user_from = _from
                            // _users_from.push(user_from.toLocaleString());
                            // var user_until = new Date(_users[i]['until']);
                            var user_until = _until
                            // _users_until.push(user_until.setHours(user_until.getHours() + 9));
                            _users_total.push((_users[i]['total_rate'] * 0.001).toFixed(3));
                            _users_download.push((_users[i]['dest_rate'] * 0.001).toFixed(3));
                            _users_upload.push((_users[i]['source_rate'] * 0.001).toFixed(3));
                            _users_active_flows.push(_users[i]['active_flows']);
                            _users_packet_disc_rate.push(_users[i]['packet_discard_rate']);
                            _users_packet_disc_rate.push(_users[i]['packets_discarded']);
                            _users_tb_data.push({
                                name: _users[i]['name'],
                                from: (_.has(_users[i], "from")) ? user_from:_from,
                                until: (_.has(_users[i], "until")) ? user_until:_until,
                                total: (_users[i]['total_rate'] * 0.001).toFixed(3),
                                down: (_users[i]['dest_rate'] * 0.001).toFixed(3),
                                up: (_users[i]['source_rate'] * 0.001).toFixed(3),
                                flows: _users[i]['active_flows'],
                                disc_rate: _users[i]['packet_discard_rate'],
                                pack_disc: _users[i]['packets_discarded']

                            });
                        }
                        console.log("user_act_flow_max_data");
                        console.log(users_act_flow_max_data);
                        _.each(_users_tb_data, function(e, i){
                            _.each(users_act_flow_max_data, function(elem, index){
                                if (index === i) {
                                    _.extend(e, {max_flows: elem[1]}, {max_flows_time: elem[2]});
                                }
                            });
                        });


                        console.log(_users_tb_data);
                        _users_data.push(_users_download);
                        _users_data.push(_users_upload);
                        _users_flow_disc_data.push(_users_active_flows);
                        _users_flow_disc_data.push(_users_packet_disc_rate);

                        var _users_option = {
                            scales: {
                                yAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: '내부사용자',
                                        fontStyle: "bold"
                                    }
                                }],
                                xAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: '사용량(Mbit/s)',
                                        fontStyle: "bold"
                                    }
                                }]
                            }
                        };

                        var _users_flow_disc_option = {
                            scales: {
                                yAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: '내부사용자',
                                        fontStyle: "bold"
                                    }
                                }],
                                xAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: '사용/제어량(/s)',
                                        fontStyle: "bold"
                                    }
                                }]
                            }
                        };
                        var _users_datasetOverride = [{
                            xAxisID: 'x-axis-1'
                        }, {
                            xAxisID: 'x-axis-2'
                        }];
                        for (var i = 0; i < _users_label.length; i++) {
                            complete_count += 1;
                            UserAppData.getUserAppData(hostname, _users_label[i]).then(function (data) {
                            // for saisei-7.3.1-11114
                            var from = new Date(data['data']['from']);
                            from.setHours(from.getHours() + 9);
                            var _from = from.toLocaleString();
                            var until = new Date(data['data']['until']);
                            until.setHours(until.getHours() + 9);
                            var _until = until.toLocaleString();
                                if (_.has(data['data']['collection'][0], "from") && _.has(data['data']['collection'][0], "until")) {
                                    var top1_from = new Date(data['data']['collection'][0]['from']);
                                    top1_from.setHours(top1_from.getHours() + 9);
                                    var top1_until = new Date(data['data']['collection'][0]['until']);
                                    top1_until.setHours(top1_until.getHours() + 9);
                                }else{
                                    var top1_from = "None";
                                    var top1_until = "None";
                                }
                                if (_.has(data['data']['collection'][1], "from") && _.has(data['data']['collection'][1], "until")) {
                                    var top2_from = new Date(data['data']['collection'][1]['from']);
                                    top2_from.setHours(top2_from.getHours() + 9);
                                    var top2_until = new Date(data['data']['collection'][1]['until']);
                                    top2_until.setHours(top2_until.getHours() + 9);
                                }else{
                                    var top2_from = "None";
                                    var top2_until = "None";
                                }
                                if (_.has(data['data']['collection'][2], "from") && _.has(data['data']['collection'][2], "until")) {
                                    var top3_from = new Date(data['data']['collection'][2]['from']);
                                    top3_from.setHours(top3_from.getHours() + 9);
                                    var top3_until = new Date(data['data']['collection'][2]['until']);
                                    top3_until.setHours(top3_until.getHours() + 9);
                                }else{
                                    var top3_from = "None";
                                    var top3_until = "None";
                                }
                                _users_app.push({
                                    "user_name": data['data']['collection'][0].link.href.split('/')[6],
                                    "top1_app_name": (_.has(data['data']['collection'][0], "name")) ? data['data']['collection'][0]['name']:'None',
                                    "top1_app_total": (_.has(data['data']['collection'][0], "total_rate")) ? (data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3):0,
                                    "top1_app_from": _from,
                                    "top1_app_until": _until,
                                    "top2_app_name": (_.has(data['data']['collection'][1], "name")) ? data['data']['collection'][1]['name']:'None',
                                    "top2_app_total": (_.has(data['data']['collection'][1], "total_rate")) ? (data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3):0,
                                    "top2_app_from": _from,
                                    "top2_app_until": _until,
                                    "top3_app_name": (_.has(data['data']['collection'][2], "name")) ? data['data']['collection'][2]['name']:'None',
                                    "top3_app_total": (_.has(data['data']['collection'][2], "total_rate")) ? (data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3):0,
                                    "top3_app_from": _from,
                                    "top3_app_until": _until
                                });
                                _users_app.sort(function (a, b) { // DESC
                                    return b['top1_app_total'] - a['top1_app_total'];
                                });
                                (_.has(data['data']['collection'][0], "total_rate"))?_users_app_top1.push((data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3)):_users_app_top1.push(0);
                                (_.has(data['data']['collection'][1], "total_rate"))?_users_app_top2.push((data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3)):_users_app_top2.push(0);
                                (_.has(data['data']['collection'][2], "total_rate"))?_users_app_top3.push((data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3)):_users_app_top3.push(0);
                                (_.has(data['data']['collection'][0], "name"))?_users_appName_top1.push((data['data']['collection'][0]['name'] * 0.001).toFixed(3)):_users_appName_top1.push(0);
                                (_.has(data['data']['collection'][1], "name"))?_users_appName_top2.push((data['data']['collection'][1]['name'] * 0.001).toFixed(3)):_users_appName_top2.push(0);
                                (_.has(data['data']['collection'][2], "name"))?_users_appName_top3.push((data['data']['collection'][2]['name'] * 0.001).toFixed(3)):_users_appName_top3.push(0);
                                if (_.has(data['data']['collection'][0], "name") && _.has(data['data']['collection'][1], "name") && _.has(data['data']['collection'][2], "name")){
                                    _users_app_label.push(data['data']['collection'][0].link.href.split('/')[6] + "(" +
                                        "1." + data['data']['collection'][0]['name'] + "," +
                                        "2." + data['data']['collection'][1]['name'] + "," +
                                        "3." + data['data']['collection'][2]['name'] + ")"
                                    );
                                }else if (_.has(data['data']['collection'][0], "name") && _.has(data['data']['collection'][1], "name")){
                                    _users_app_label.push(data['data']['collection'][0].link.href.split('/')[6] + "(" +
                                        "1." + data['data']['collection'][0]['name'] + "," +
                                        "2." + data['data']['collection'][1]['name'] + ")"
                                    );
                                }else if (_.has(data['data']['collection'][0], "name") && _.has(data['data']['collection'][2], "name")){
                                    _users_app_label.push(data['data']['collection'][0].link.href.split('/')[6] + "(" +
                                        "1." + data['data']['collection'][0]['name'] + "," +
                                        "3." + data['data']['collection'][2]['name'] + ")"
                                    );
                                }else if (_.has(data['data']['collection'][1], "name") && _.has(data['data']['collection'][2], "name")){
                                    _users_app_label.push(data['data']['collection'][1].link.href.split('/')[6] + "(" +
                                        "2." + data['data']['collection'][1]['name'] + "," +
                                        "3." + data['data']['collection'][2]['name'] + ")"
                                    );
                                }else if (_.has(data['data']['collection'][0], "name")){
                                    _users_app_label.push(data['data']['collection'][0].link.href.split('/')[6] + "(" +
                                        "1." + data['data']['collection'][0]['name'] + ")"
                                    );
                                }else if (_.has(data['data']['collection'][1], "name")){
                                    _users_app_label.push(data['data']['collection'][1].link.href.split('/')[6] + "(" +
                                        "2." + data['data']['collection'][1]['name'] + ")"
                                    );
                                }else{
                                    _users_app_label.push(data['data']['collection'][2].link.href.split('/')[6] + "(" +
                                        "3." + data['data']['collection'][2]['name'] + ")"
                                    );
                                }
                            });
                        }
                        var _users_app_data = [
                            _users_app_top1,
                            _users_app_top2,
                            _users_app_top3
                        ];
                        var _users_app_series = ["TOP APP 1", "TOP APP 2", "TOP APP 3"];
                        var _users_app_option = {
                            scales: {
                                xAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: 'APP 사용량(Mbit/s)',
                                        fontStyle: "bold"
                                    }
                                }],
                                yAxes: [{
                                    ticks: {
                                        fontSize: 12,
                                        fontStyle: "bold"
                                    },
                                    scaleLabel: {
                                        display: true,
                                        fontSize: 14,
                                        labelString: '사용자 어플리케이션(Top1,Top2,Top3)',
                                        fontStyle: "bold"
                                    }
                                }]
                            }
                        };
                        deferred.resolve({
                            user: {
                                _users_tb_data: _users_tb_data, // for table
                                _users_data: _users_data,
                                _users_flow_disc_data: _users_flow_disc_data,
                                _users_label: _users_label,
                                _users_series: _users_series,
                                _users_flow_disc_series: _users_flow_disc_series,
                                _users_option: _users_option,
                                _users_flow_disc_option: _users_flow_disc_option,
                                _users_datasetOverride: _users_datasetOverride,
                                colors: colors
                            },
                            user_app: {
                                _users_app: _users_app, // for table
                                _users_app_data: _users_app_data,
                                _users_app_label: _users_app_label,
                                _users_app_series: _users_app_series,
                                _users_app_option: _users_app_option,
                                colors: colors
                            },
                            complete_count: complete_count
                        });
                    });
                });

            }
            return deferred.promise;
        };
        this.q_userFlowData = function(hostname, from, until, duration, isset){
            var deferred = $q.defer();
            var from = from;
            var until = until;
            var duration = duration;
            if (isset) {
                ReportData.getUserActiveFlows(hostname).then(function (data) {
                    console.log(data);
                    deferred.resolve("resolve!");
                });
            }
            return deferred.promise;
        }
    };
    return UserData;
});
reportApp.service('ReportUserGroupData', function($window, $q, _, ReportData, UserInGroupData, SharedData) {
    var UserGroupData = function() {
        var self = this;
        this.q_userGroupData = function(hostname, from, until, duration, isset) {
            var deferred = $q.defer();
            var from = from;
            var until = until;
            var duration = duration;
            var complete_count = 0;
            if (isset) {
                ReportData.getUserGroupSize(hostname).then(function (size) {
                    complete_count += 1;
                    console.log(size.group_size);
                    ReportData.getUserGroupActiveFlows(hostname, size.group_size).then(function(group_flow_data){
                        complete_count += 1;
                        var _history_user_group_active_flows_data = group_flow_data['data']['collection'];
                        var user_group_act_flow_max_data = [];

                        _.each(_history_user_group_active_flows_data, function(collection) {

                            user_group_act_flow_max_data.push(_.max(collection['_history_active_flows'], function(history_active_flows){
                                return history_active_flows[1];
                            }));
                        });
                        _.each(user_group_act_flow_max_data, function(elem, index, data){
                            var t = new Date(elem[0]);
                            elem.push(t.toLocaleString());
                        });

                        ReportData.getUserGroupData(hostname, size.group_size).then(function (data) {
                            complete_count += 1;
                            var _user_group_label = [];
                            var _user_group_from = [];
                            var _user_group_until = [];
                            var _user_group_series = ['다운로드 사용량(Mbit/s)', '업로드 사용량(Mbit/s)'];
                            var _user_group_flow_disc_series = ['플로우 사용량(/s)', '제어량(/s)'];
                            var _user_group_total = [];
                            var _user_group_download = [];
                            var _user_group_upload = [];
                            var _user_group_active_flows = [];
                            var _user_group_packet_disc = [];
                            var _user_group_packet_disc_rate = [];
                            var _user_group_tb_data = [];
                            var _user_group_data = [];
                            var _user_group_flow_disc_data = [];
                            var _user_group_colors = ['#ff6384', '#45b7cd', '#ffe200'];
                            var _user_groups = data['data']['collection'];
                            var _user_group_size = data['data']['size'];
                            //for 7.3.1 11114 version of saisei
                            var user_group_from = new Date(data['data']['from']);
                            user_group_from.setHours(user_group_from.getHours() + 9);
                            _user_group_from.push(user_group_from.toLocaleString());
                            var user_group_until = new Date(data['data']['until']);
                            user_group_until.setHours(user_group_until.getHours() + 9);
                            _user_group_until.push(user_group_until.toLocaleString());
                            _(_user_groups).each(function(elem, index){
                                _user_group_label.push(elem.name);
                                // var user_group_from = new Date(elem.from);
                                // user_group_from.setHours(user_group_from.getHours() + 9);
                                // _user_group_from.push(user_group_from.toLocaleString());
                                // var user_group_until = new Date(elem.until);
                                // user_group_until.setHours(user_group_until.getHours() + 9);
                                // _user_group_until.push(user_group_until.toLocaleString());
                                _user_group_total.push((elem.total_rate * 0.001).toFixed(3));
                                _user_group_download.push((elem.dest_rate * 0.001).toFixed(3));
                                _user_group_upload.push((elem.source_rate * 0.001).toFixed(3));
                                _user_group_active_flows.push(elem.active_flows);
                                _user_group_packet_disc_rate.push(elem.packet_discard_rate);
                                _user_group_packet_disc.push(elem.packets_discarded);
                                _user_group_tb_data.push({
                                    name: elem.name,
                                    from: user_group_from.toLocaleString(),
                                    until: user_group_until.toLocaleString(),
                                    total: (elem.total_rate * 0.001).toFixed(3),
                                    down: (elem.dest_rate * 0.001).toFixed(3),
                                    up: (elem.source_rate * 0.001).toFixed(3),
                                    flows: elem.active_flows,
                                    disc_rate: elem.packet_discard_rate,
                                    pack_disc: elem.packets_discarded
                                });
                            });
                            _user_group_data.push(_user_group_download);
                            _user_group_data.push(_user_group_upload);
                            console.log("user_group_act_flow_max_data", user_group_act_flow_max_data);
                            _.each(_user_group_tb_data, function(e, i){
                                _.each(user_group_act_flow_max_data, function(elem, index){
                                    if (index === i) {
                                        _.extend(e, {max_flows: elem[1]}, {max_flows_time: elem[2]});
                                    }
                                });
                            });
                            var _user_group_option = {
                                scales: {
                                    yAxes: [{
                                        ticks: {
                                            fontSize: 12,
                                            fontStyle: "bold"
                                        },
                                        scaleLabel: {
                                            display: true,
                                            fontSize: 14,
                                            labelString: '사용자 그룹',
                                            fontStyle: "bold"
                                        }
                                    }],
                                    xAxes: [{
                                        ticks: {
                                            fontSize: 12,
                                            fontStyle: "bold"
                                        },
                                        scaleLabel: {
                                            display: true,
                                            fontSize: 14,
                                            labelString: '트래픽 사용량(Mbit/s)',
                                            fontStyle: "bold"
                                        }
                                    }]
                                }
                            };
                            var _user_in_group_tb = [];
                            var _group_name_tb = [];
                            var _user_in_group_tr_top1=[], _user_in_group_tr_top2=[], _user_in_group_tr_top3=[],
                                _user_in_group_tr_top4=[], _user_in_group_tr_top5=[];
                            var _user_in_group_top1_dataset=[], _user_in_group_top2_dataset=[],_user_in_group_top3_dataset=[],
                                _user_in_group_top4_dataset=[],_user_in_group_top5_dataset=[];
                            var _user_in_group_label = [];
                            var user_names_inGroup = [];
                            var _user_inGroup_tr_top1 = [];
                            var user_inGroup_tb_count=0;
                            var _user_in_group_tb_count=0;
                            _(_user_group_label).each(function(elem, index) {
                                complete_count += 1;
                                _group_name_tb.push(elem);
                                UserInGroupData.getUserInGroupData(hostname, elem).then(function(data) {
                                    var top1_user_from, top2_user_from, top3_user_from, top4_user_from, top5_user_from, top_user_from;
                                    var top1_user_until, top2_user_until, top3_user_until, top4_user_until, top5_user_until, top_user_until;
                                    var top1_user_name, top2_user_name, top3_user_name,top4_user_name,top5_user_name;
                                    var top1_user_total,top2_user_total,top3_user_total,top4_user_total,top5_user_total;
                                    var top1_user_down,top2_user_down,top3_user_down,top4_user_down,top5_user_down;
                                    var top1_user_up,top2_user_up,top3_user_up,top4_user_up,top5_user_up;
                                    var top1_user_active_flow,top2_user_active_flow,top3_user_active_flow,top4_user_active_flow,top5_user_active_flow;
                                    var top1_user_packet_disc_rate,top2_user_packet_disc_rate,top3_user_packet_disc_rate,top4_user_packet_disc_rate,top5_user_packet_disc_rate;
                                    var top1_user_packet_disc,top2_user_packet_disc,top3_user_packet_disc,top4_user_packet_disc,top5_user_packet_disc;
                                    var top1_user_max_flow_data, top2_user_max_flow_data, top3_user_max_flow_data, top4_user_max_flow_data, top5_user_max_flow_data;
                                    var top1_user_max_flow, top2_user_max_flow, top3_user_max_flow, top4_user_max_flow, top5_user_max_flow;
                                    var top1_user_max_flow_time,top2_user_max_flow_time,top3_user_max_flow_time,top4_user_max_flow_time,top5_user_max_flow_time;
                                    var top1_user_app1_name, top1_user_app2_name, top1_user_app3_name;
                                    var top1_user_app1_total, top1_user_app2_total, top1_user_app3_total;
                                    var top2_user_app1_name, top2_user_app2_name, top2_user_app3_name;
                                    var top2_user_app1_total, top2_user_app2_total, top2_user_app3_total;
                                    var top3_user_app1_name, top3_user_app2_name, top3_user_app3_name;
                                    var top3_user_app1_total, top3_user_app2_total, top3_user_app3_total;
                                    var top4_user_app1_name, top4_user_app2_name, top4_user_app3_name;
                                    var top4_user_app1_total, top4_user_app2_total, top4_user_app3_total;
                                    var top5_user_app1_name, top5_user_app2_name, top5_user_app3_name;
                                    var top5_user_app1_total, top5_user_app2_total, top5_user_app3_total;
                                    var top_user_data = [];
                                    var top_user_app = [];
                                    var resolve_cnt = 0;
                                    var makeTableData = function(){
                                        return $q(function(resolve, reject){
                                            if (data['data']['collection'].length !== 0){
                                                //for 7.3.1 11114 version of saisei
                                                try {
                                                    top_user_from = new Date(data['data']['from']);
                                                    top_user_from.setHours(top_user_from.getHours() + 9);
                                                    top_user_from = top_user_from.toLocaleString();
                                                    top_user_until = new Date(data['data']['until']);
                                                    top_user_until.setHours(top_user_until.getHours() + 9);
                                                    top_user_until = top_user_until.toLocaleString();
                                                } catch(exception) {top_user_from = 'None'; top_user_until='None'}
                                                _(data['data']['collection']).each(function(e, i){
                                                    complete_count += 2;
                                                    if(i===0) {
                                                        complete_count += 2;
                                                        UserInGroupData.getUserInGroupActiveFlows(hostname, e['name']).then(function(user_in_group_flow_data){
                                                            UserInGroupData.getUserInGroupAppData(hostname, e['name']).then(function(user_in_group_app_data){
                                                                try {
                                                                    top1_user_from = new Date(e['from']);
                                                                    top1_user_from.setHours(top1_user_from.getHours() + 9);
                                                                    top1_user_from = top1_user_from.toLocaleString();
                                                                    top1_user_until = new Date(e['until']);
                                                                    top1_user_until.setHours(top1_user_until.getHours() + 9);
                                                                    top1_user_until = top1_user_until.toLocaleString();
                                                                } catch(exception) {top1_user_from = 'None'; top1_user_until='None'}
                                                                try { top1_user_name = e['name'];} catch(exception) { top1_user_name = 'None' }
                                                                try { top1_user_total = (e['total_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_total = 0 }
                                                                try { top1_user_down = (e['dest_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_down = 0 }
                                                                try { top1_user_up = (e['source_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_up = 0 }
                                                                try { top1_user_active_flow = e['active_flows']; } catch(exception) { top1_user_active_flow = 0 }
                                                                try { top1_user_packet_disc_rate = e['packet_discard_rate']; } catch(exception) { top1_user_packet_disc_rate = 0 }
                                                                try { top1_user_packet_disc = e['packets_discarded']; } catch(exception) { top1_user_packet_disc = 0 }
                                                                try {
                                                                    var _history_user_in_group_active_flows_data = user_in_group_flow_data['data']['collection'][0]['_history_active_flows'];
                                                                    top1_user_max_flow_data=_.max(_history_user_in_group_active_flows_data,
                                                                        function (history_user_in_grp_active_flows) {
                                                                            return history_user_in_grp_active_flows[1];
                                                                        });
                                                                    top1_user_max_flow = top1_user_max_flow_data[1];
                                                                    top1_user_max_flow_time = (new Date(top1_user_max_flow_data[0])).toLocaleString();
                                                                } catch(exception) {
                                                                    top1_user_max_flow = 0; top1_user_max_flow_time = "none";
                                                                }
                                                                try {
                                                                    var top1_user_app1_from = new Date(user_in_group_app_data['data']['collection'][0]['from']);
                                                                    top1_user_app1_from.setHours(top1_user_app1_from.getHours() + 9);
                                                                    var top1_user_app1_until = new Date(user_in_group_app_data['data']['collection'][0]['until']);
                                                                    top1_user_app1_until.setHours(top1_user_app1_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top1_user_app1_from = "None";
                                                                    var top1_user_app1_until = "None";
                                                                }
                                                                try {
                                                                    var top1_user_app2_from = new Date(user_in_group_app_data['data']['collection'][1]['from']);
                                                                    top1_user_app2_from.setHours(top1_user_app2_from.getHours() + 9);
                                                                    var top1_user_app2_until = new Date(user_in_group_app_data['data']['collection'][1]['until']);
                                                                    top1_user_app2_until.setHours(top1_user_app2_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top1_user_app2_from = "None";
                                                                    var top1_user_app2_until = "None";
                                                                }
                                                                try {
                                                                    var top1_user_app3_from = new Date(user_in_group_app_data['data']['collection'][2]['from']);
                                                                    top1_user_app3_from.setHours(top1_user_app3_from.getHours() + 9);
                                                                    var top1_user_app3_until = new Date(user_in_group_app_data['data']['collection'][2]['until']);
                                                                    top1_user_app3_until.setHours(top1_user_app3_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top1_user_app3_from = "None";
                                                                    var top1_user_app3_until = "None";
                                                                }
                                                                try { top1_user_app1_name = user_in_group_app_data['data']['collection'][0]['name'];} catch(exception) { top1_user_app1_name = 'None' }
                                                                try { top1_user_app1_total = (user_in_group_app_data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_app1_total = 0 }
                                                                try { top1_user_app2_name = user_in_group_app_data['data']['collection'][1]['name'];} catch(exception) { top1_user_app2_name = 'None' }
                                                                try { top1_user_app2_total = (user_in_group_app_data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_app2_total = 0 }
                                                                try { top1_user_app3_name = user_in_group_app_data['data']['collection'][2]['name'];} catch(exception) { top1_user_app3_name = 'None' }
                                                                try { top1_user_app3_total = (user_in_group_app_data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top1_user_app3_total = 0 }
                                                                top_user_data.push({
                                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top1_user_name:"None",
                                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top1_user_total:0,
                                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top1_user_down:0,
                                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top1_user_up:0,
                                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top1_user_active_flow:0,
                                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top1_user_packet_disc_rate:0,
                                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top1_user_packet_disc:0,
                                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top1_user_max_flow:0,
                                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top1_user_max_flow_time:0,
                                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top_user_from:"None",
                                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top_user_until:"None",
                                                                    "top_user_app1_name": top1_user_app1_name,
                                                                    "top_user_app1_total": top1_user_app1_total,
                                                                    "top_user_app1_from": top_user_from,
                                                                    "top_user_app1_until": top_user_until,
                                                                    "top_user_app2_name": top1_user_app2_name,
                                                                    "top_user_app2_total": top1_user_app2_total,
                                                                    "top_user_app2_from": top_user_from,
                                                                    "top_user_app2_until": top_user_until,
                                                                    "top_user_app3_name": top1_user_app3_name,
                                                                    "top_user_app3_total": top1_user_app3_total,
                                                                    "top_user_app3_from": top_user_from,
                                                                    "top_user_app3_until": top_user_until
                                                                });
                                                                resolve("sucess make table data!");
                                                                resolve_cnt += 1;
                                                            });
                                                        });
                                                    }
                                                    if(i===1) {
                                                        complete_count += 2;
                                                        UserInGroupData.getUserInGroupActiveFlows(hostname, e['name']).then(function(user_in_group_flow_data){
                                                            UserInGroupData.getUserInGroupAppData(hostname, e['name']).then(function(user_in_group_app_data){
                                                                try {
                                                                    top2_user_from = new Date(e['from']);
                                                                    top2_user_from.setHours(top2_user_from.getHours() + 9);
                                                                    top2_user_from = top2_user_from.toLocaleString();
                                                                    top2_user_until = new Date(e['until']);
                                                                    top2_user_until.setHours(top2_user_until.getHours() + 9);
                                                                    top2_user_until = top2_user_until.toLocaleString();

                                                                } catch(exception) {top2_user_from = 'None'; top2_user_until='None'}
                                                                try { top2_user_name = e['name'];} catch(exception) { top2_user_name = 'None' }
                                                                try { top2_user_total = (e['total_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_total = 0 }
                                                                try { top2_user_down = (e['dest_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_total = 0 }
                                                                try { top2_user_up = (e['source_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_total = 0 }
                                                                try { top2_user_active_flow = e['active_flows']; } catch(exception) { top2_user_total = 0 }
                                                                try { top2_user_packet_disc_rate = e['packet_discard_rate']; } catch(exception) { top2_user_total = 0 }
                                                                try { top2_user_packet_disc = e['packets_discarded']; } catch(exception) { top2_user_packet_disc = 0 }
                                                                try {
                                                                    var _history_user_in_group_active_flows_data = user_in_group_flow_data['data']['collection'][0]['_history_active_flows'];
                                                                    top2_user_max_flow_data=_.max(_history_user_in_group_active_flows_data,
                                                                        function (history_user_in_grp_active_flows) {
                                                                            return history_user_in_grp_active_flows[1];
                                                                        });
                                                                    top2_user_max_flow = top2_user_max_flow_data[1];
                                                                    top2_user_max_flow_time = (new Date(top2_user_max_flow_data[0])).toLocaleString();

                                                                } catch(exception) {
                                                                    top2_user_max_flow = 0; top2_user_max_flow_time = "none";
                                                                }
                                                                try {
                                                                    var top2_user_app1_from = new Date(user_in_group_app_data['data']['collection'][0]['from']);
                                                                    top2_user_app1_from.setHours(top2_user_app1_from.getHours() + 9);
                                                                    var top2_user_app1_until = new Date(user_in_group_app_data['data']['collection'][0]['until']);
                                                                    top2_user_app1_until.setHours(top2_user_app1_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top2_user_app1_from = "None";
                                                                    var top2_user_app1_until = "None";
                                                                }
                                                                try {
                                                                    var top2_user_app2_from = new Date(user_in_group_app_data['data']['collection'][1]['from']);
                                                                    top2_user_app2_from.setHours(top2_user_app2_from.getHours() + 9);
                                                                    var top2_user_app2_until = new Date(user_in_group_app_data['data']['collection'][1]['until']);
                                                                    top2_user_app2_until.setHours(top2_user_app2_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top2_user_app2_from = "None";
                                                                    var top2_user_app2_until = "None";
                                                                }
                                                                try {
                                                                    var top2_user_app3_from = new Date(user_in_group_app_data['data']['collection'][2]['from']);
                                                                    top2_user_app3_from.setHours(top2_user_app3_from.getHours() + 9);
                                                                    var top2_user_app3_until = new Date(user_in_group_app_data['data']['collection'][2]['until']);
                                                                    top2_user_app3_until.setHours(top2_user_app3_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top2_user_app3_from = "None";
                                                                    var top2_user_app3_until = "None";
                                                                }
                                                                try { top2_user_app1_name = user_in_group_app_data['data']['collection'][0]['name'];} catch(exception) { top2_user_app1_name = 'None' }
                                                                try { top2_user_app1_total = (user_in_group_app_data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_app1_total = 0}
                                                                try { top2_user_app2_name = user_in_group_app_data['data']['collection'][1]['name'];} catch(exception) { top2_user_app2_name = 'None' }
                                                                try { top2_user_app2_total = (user_in_group_app_data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_app2_total = 0}
                                                                try { top2_user_app3_name = user_in_group_app_data['data']['collection'][2]['name'];} catch(exception) { top2_user_app3_name = 'None' }
                                                                try { top2_user_app3_total = (user_in_group_app_data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top2_user_app3_total = 0 }
                                                                top_user_data.push({
                                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top2_user_name:"None",
                                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top2_user_total:0,
                                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top2_user_down:0,
                                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top2_user_up:0,
                                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top2_user_active_flow:0,
                                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top2_user_packet_disc_rate:0,
                                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top2_user_packet_disc:0,
                                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top2_user_max_flow:0,
                                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top2_user_max_flow_time:0,
                                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top_user_from:"None",
                                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top_user_until:"None",
                                                                    "top_user_app1_name": top1_user_app1_name,
                                                                    "top_user_app1_total": top1_user_app1_total,
                                                                    "top_user_app1_from": top_user_from,
                                                                    "top_user_app1_until": top_user_until,
                                                                    "top_user_app2_name": top1_user_app2_name,
                                                                    "top_user_app2_total": top1_user_app2_total,
                                                                    "top_user_app2_from": top_user_from,
                                                                    "top_user_app2_until": top_user_until,
                                                                    "top_user_app3_name": top1_user_app3_name,
                                                                    "top_user_app3_total": top1_user_app3_total,
                                                                    "top_user_app3_from": top_user_from,
                                                                    "top_user_app3_until": top_user_until
                                                                });
                                                                resolve_cnt += 1;
                                                            });
                                                        });
                                                    }
                                                    if(i===2) {
                                                        complete_count += 2;
                                                        UserInGroupData.getUserInGroupActiveFlows(hostname, e['name']).then(function(user_in_group_flow_data){
                                                            UserInGroupData.getUserInGroupAppData(hostname, e['name']).then(function(user_in_group_app_data) {
                                                                try {
                                                                    top3_user_from = new Date(e['from']);
                                                                    top3_user_from.setHours(top3_user_from.getHours() + 9);
                                                                    top3_user_from = top3_user_from.toLocaleString();

                                                                    top3_user_until = new Date(e['until']);
                                                                    top3_user_until.setHours(top3_user_until.getHours() + 9);
                                                                    top3_user_until = top3_user_until.toLocaleString();

                                                                } catch(exception) {top3_user_from = 'None'; top3_user_until='None'}

                                                                try { top3_user_name = e['name'];} catch(exception) { top3_user_name = 'None'; }
                                                                try { top3_user_total = (e['total_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_total = 0; }
                                                                try { top3_user_down = (e['dest_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_total = 0 }
                                                                try { top3_user_up = (e['source_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_total = 0 }
                                                                try { top3_user_active_flow = e['active_flows']; } catch(exception) { top3_user_total = 0 }
                                                                try { top3_user_packet_disc_rate = e['packet_discard_rate']; } catch(exception) { top3_user_total = 0 }
                                                                try { top3_user_packet_disc = e['packets_discarded']; } catch(exception) { top3_user_packet_disc = 0 }
                                                                try {
                                                                    var _history_user_in_group_active_flows_data = user_in_group_flow_data['data']['collection'][0]['_history_active_flows'];
                                                                    top3_user_max_flow_data=_.max(_history_user_in_group_active_flows_data,
                                                                        function (history_user_in_grp_active_flows) {
                                                                            return history_user_in_grp_active_flows[1];
                                                                        });
                                                                    top3_user_max_flow = top3_user_max_flow_data[1];
                                                                    top3_user_max_flow_time = (new Date(top3_user_max_flow_data[0])).toLocaleString();
                                                                } catch(exception) {
                                                                    top3_user_max_flow = 0; top3_user_max_flow_time = "none";
                                                                }
                                                                try {
                                                                    var top3_user_app1_from = new Date(user_in_group_app_data['data']['collection'][0]['from']);
                                                                    top3_user_app1_from.setHours(top3_user_app1_from.getHours() + 9);
                                                                    var top3_user_app1_until = new Date(user_in_group_app_data['data']['collection'][0]['until']);
                                                                    top3_user_app1_until.setHours(top3_user_app1_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top3_user_app1_from = "None";
                                                                    var top3_user_app1_until = "None";
                                                                }
                                                                try {
                                                                    var top3_user_app2_from = new Date(user_in_group_app_data['data']['collection'][1]['from']);
                                                                    top3_user_app2_from.setHours(top3_user_app2_from.getHours() + 9);
                                                                    var top3_user_app2_until = new Date(user_in_group_app_data['data']['collection'][1]['until']);
                                                                    top3_user_app2_until.setHours(top3_user_app2_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top3_user_app2_from = "None";
                                                                    var top3_user_app2_until = "None";
                                                                }
                                                                try {
                                                                    var top3_user_app3_from = new Date(user_in_group_app_data['data']['collection'][2]['from']);
                                                                    top3_user_app3_from.setHours(top3_user_app3_from.getHours() + 9);
                                                                    var top3_user_app3_until = new Date(user_in_group_app_data['data']['collection'][2]['until']);
                                                                    top3_user_app3_until.setHours(top3_user_app3_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top3_user_app3_from = "None";
                                                                    var top3_user_app3_until = "None";
                                                                }
                                                                try { top3_user_app1_name = user_in_group_app_data['data']['collection'][0]['name'];} catch(exception) { top3_user_app1_name = 'None' }
                                                                try { top3_user_app1_total = (user_in_group_app_data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_app1_total = 0 }
                                                                try { top3_user_app2_name = user_in_group_app_data['data']['collection'][1]['name'];} catch(exception) { top3_user_app2_name = 'None' }
                                                                try { top3_user_app2_total = (user_in_group_app_data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_app2_total = 0 }
                                                                try { top3_user_app3_name = user_in_group_app_data['data']['collection'][2]['name'];} catch(exception) { top3_user_app3_name = 'None' }
                                                                try { top3_user_app3_total = (user_in_group_app_data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top3_user_app3_total = 0 }
                                                                top_user_data.push({
                                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top3_user_name:"None",
                                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top3_user_total:0,
                                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top3_user_down:0,
                                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top3_user_up:0,
                                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top3_user_active_flow:0,
                                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top3_user_packet_disc_rate:0,
                                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top3_user_packet_disc:0,
                                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top3_user_max_flow:0,
                                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top3_user_max_flow_time:0,
                                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top_user_from:"None",
                                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top_user_until:"None",
                                                                    "top_user_app1_name": top1_user_app1_name,
                                                                    "top_user_app1_total": top1_user_app1_total,
                                                                    "top_user_app1_from": top_user_from,
                                                                    "top_user_app1_until": top_user_until,
                                                                    "top_user_app2_name": top1_user_app2_name,
                                                                    "top_user_app2_total": top1_user_app2_total,
                                                                    "top_user_app2_from": top_user_from,
                                                                    "top_user_app2_until": top_user_until,
                                                                    "top_user_app3_name": top1_user_app3_name,
                                                                    "top_user_app3_total": top1_user_app3_total,
                                                                    "top_user_app3_from": top_user_from,
                                                                    "top_user_app3_until": top_user_until
                                                                });
                                                                resolve_cnt += 1;
                                                            });
                                                        });
                                                    }
                                                    if(i===3) {
                                                        complete_count += 2;
                                                        UserInGroupData.getUserInGroupActiveFlows(hostname, e['name']).then(function(user_in_group_flow_data){
                                                            UserInGroupData.getUserInGroupAppData(hostname, e['name']).then(function(user_in_group_app_data) {
                                                                try {
                                                                    top4_user_from = new Date(e['from']);
                                                                    top4_user_from.setHours(top4_user_from.getHours() + 9);
                                                                    top4_user_from = top4_user_from.toLocaleString();

                                                                    top4_user_until = new Date(e['until']);
                                                                    top4_user_until.setHours(top4_user_until.getHours() + 9);
                                                                    top4_user_until = top4_user_until.toLocaleString();

                                                                } catch(exception) {top4_user_from = 'None'; top4_user_until='None'}

                                                                try { top4_user_name = e['name'];} catch(exception) { top4_user_name = 'None'; }
                                                                try { top4_user_total = (e['total_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_total = 0; }
                                                                try { top4_user_down = (e['dest_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_down = 0 }
                                                                try { top4_user_up = (e['source_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_up = 0 }
                                                                try { top4_user_active_flow = e['active_flows']; } catch(exception) { top4_user_active_flow = 0 }
                                                                try { top4_user_packet_disc_rate = e['packet_discard_rate']; } catch(exception) { top4_user_packet_disc_rate = 0 }
                                                                try { top4_user_packet_disc = e['packets_discarded']; } catch(exception) { top4_user_packet_disc = 0 }
                                                                try {
                                                                    var _history_user_in_group_active_flows_data = user_in_group_flow_data['data']['collection'][0]['_history_active_flows'];
                                                                    top4_user_max_flow_data=_.max(_history_user_in_group_active_flows_data,
                                                                        function (history_user_in_grp_active_flows) {
                                                                            return history_user_in_grp_active_flows[1];
                                                                        });
                                                                    top4_user_max_flow = top4_user_max_flow_data[1];
                                                                    top4_user_max_flow_time = (new Date(top4_user_max_flow_data[0])).toLocaleString();
                                                                } catch(exception) {
                                                                    top4_user_max_flow = 0; top4_user_max_flow_time = "none";
                                                                }
                                                                try {
                                                                    var top4_user_app1_from = new Date(user_in_group_app_data['data']['collection'][0]['from']);
                                                                    top4_user_app1_from.setHours(top4_user_app1_from.getHours() + 9);
                                                                    var top4_user_app1_until = new Date(user_in_group_app_data['data']['collection'][0]['until']);
                                                                    top4_user_app1_until.setHours(top4_user_app1_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top4_user_app1_from = "None";
                                                                    var top4_user_app1_until = "None";
                                                                }
                                                                try {
                                                                    var top4_user_app2_from = new Date(user_in_group_app_data['data']['collection'][1]['from']);
                                                                    top4_user_app2_from.setHours(top4_user_app2_from.getHours() + 9);
                                                                    var top4_user_app2_until = new Date(user_in_group_app_data['data']['collection'][1]['until']);
                                                                    top4_user_app2_until.setHours(top4_user_app2_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top4_user_app2_from = "None";
                                                                    var top4_user_app2_until = "None";
                                                                }
                                                                try {
                                                                    var top4_user_app3_from = new Date(user_in_group_app_data['data']['collection'][2]['from']);
                                                                    top4_user_app3_from.setHours(top4_user_app3_from.getHours() + 9);
                                                                    var top4_user_app3_until = new Date(user_in_group_app_data['data']['collection'][2]['until']);
                                                                    top4_user_app3_until.setHours(top4_user_app3_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top4_user_app3_from = "None";
                                                                    var top4_user_app3_until = "None";
                                                                }
                                                                try { top4_user_app1_name = user_in_group_app_data['data']['collection'][0]['name'];} catch(exception) { top4_user_app1_name = 'None' }
                                                                try { top4_user_app1_total = (user_in_group_app_data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_app1_total = 0 }
                                                                try { top4_user_app2_name = user_in_group_app_data['data']['collection'][1]['name'];} catch(exception) { top4_user_app2_name = 'None' }
                                                                try { top4_user_app2_total = (user_in_group_app_data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_app2_total = 0 }
                                                                try { top4_user_app3_name = user_in_group_app_data['data']['collection'][2]['name'];} catch(exception) { top4_user_app3_name = 'None' }
                                                                try { top4_user_app3_total = (user_in_group_app_data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top4_user_app3_total = 0 }
                                                                top_user_data.push({
                                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top4_user_name:"None",
                                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top4_user_total:0,
                                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top4_user_down:0,
                                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top4_user_up:0,
                                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top4_user_active_flow:0,
                                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top4_user_packet_disc_rate:0,
                                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top4_user_packet_disc:0,
                                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top4_user_max_flow:0,
                                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top4_user_max_flow_time:0,
                                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top_user_from:"None",
                                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top_user_until:"None",
                                                                    "top_user_app1_name": top1_user_app1_name,
                                                                    "top_user_app1_total": top1_user_app1_total,
                                                                    "top_user_app1_from": top_user_from,
                                                                    "top_user_app1_until": top_user_until,
                                                                    "top_user_app2_name": top1_user_app2_name,
                                                                    "top_user_app2_total": top1_user_app2_total,
                                                                    "top_user_app2_from": top_user_from,
                                                                    "top_user_app2_until": top_user_until,
                                                                    "top_user_app3_name": top1_user_app3_name,
                                                                    "top_user_app3_total": top1_user_app3_total,
                                                                    "top_user_app3_from": top_user_from,
                                                                    "top_user_app3_until": top_user_until
                                                                });
                                                                resolve_cnt += 1;
                                                            });
                                                        });
                                                    }
                                                    if(i===4) {
                                                        complete_count += 2;
                                                        UserInGroupData.getUserInGroupActiveFlows(hostname, e['name']).then(function(user_in_group_flow_data){
                                                            UserInGroupData.getUserInGroupAppData(hostname, e['name']).then(function(user_in_group_app_data) {
                                                                try {
                                                                    var top5_user_app1_from = new Date(user_in_group_app_data['data']['collection'][0]['from']);
                                                                    top5_user_app1_from.setHours(top5_user_app1_from.getHours() + 9);
                                                                    var top5_user_app1_until = new Date(user_in_group_app_data['data']['collection'][0]['until']);
                                                                    top5_user_app1_until.setHours(top5_user_app1_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top5_user_app1_from = "None";
                                                                    var top5_user_app1_until = "None";
                                                                }
                                                                try {
                                                                    var top5_user_app2_from = new Date(user_in_group_app_data['data']['collection'][1]['from']);
                                                                    top5_user_app2_from.setHours(top5_user_app2_from.getHours() + 9);
                                                                    var top5_user_app2_until = new Date(user_in_group_app_data['data']['collection'][1]['until']);
                                                                    top5_user_app2_until.setHours(top5_user_app2_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top5_user_app2_from = "None";
                                                                    var top5_user_app2_until = "None";
                                                                }
                                                                try {
                                                                    var top5_user_app3_from = new Date(user_in_group_app_data['data']['collection'][2]['from']);
                                                                    top5_user_app3_from.setHours(top5_user_app3_from.getHours() + 9);
                                                                    var top5_user_app3_until = new Date(user_in_group_app_data['data']['collection'][2]['until']);
                                                                    top5_user_app3_until.setHours(top5_user_app3_until.getHours() + 9);
                                                                } catch(exception){
                                                                    var top5_user_app3_from = "None";
                                                                    var top5_user_app3_until = "None";
                                                                }
                                                                try { top5_user_app1_name = user_in_group_app_data['data']['collection'][0]['name'];} catch(exception) { top5_user_app1_name = 'None' }
                                                                try { top5_user_app1_total = (user_in_group_app_data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_app1_total = 0 }
                                                                try { top5_user_app2_name = user_in_group_app_data['data']['collection'][1]['name'];} catch(exception) { top5_user_app2_name = 'None' }
                                                                try { top5_user_app2_total = (user_in_group_app_data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_app2_total = 0 }
                                                                try { top5_user_app3_name = user_in_group_app_data['data']['collection'][2]['name'];} catch(exception) { top5_user_app3_name = 'None' }
                                                                try { top5_user_app3_total = (user_in_group_app_data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_app3_total = 0 }

                                                                try {
                                                                    top5_user_from = new Date(e['from']);
                                                                    top5_user_from.setHours(top5_user_from.getHours() + 9);
                                                                    top5_user_from = top5_user_from.toLocaleString();

                                                                    top5_user_until = new Date(e['until']);
                                                                    top5_user_until.setHours(top5_user_until.getHours() + 9);
                                                                    top5_user_until = top5_user_until.toLocaleString();

                                                                } catch(exception) {top5_user_from = 'None'; top5_user_until='None'}

                                                                try { top5_user_name = e['name'];} catch(exception) { top5_user_name = 'None'; }
                                                                try { top5_user_total = (e['total_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_total = 0; }
                                                                try { top5_user_down = (e['dest_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_down = 0 }
                                                                try { top5_user_up = (e['source_rate'] * 0.001).toFixed(3);} catch(exception) { top5_user_up = 0 }
                                                                try { top5_user_active_flow = e['active_flows']; } catch(exception) { top5_user_active_flow = 0 }
                                                                try { top5_user_packet_disc_rate = e['packet_discard_rate']; } catch(exception) { top5_user_packet_disc_rate = 0 }
                                                                try { top5_user_packet_disc = e['packets_discarded']; } catch(exception) { top5_user_packet_disc = 0 }
                                                                try {
                                                                    var _history_user_in_group_active_flows_data = user_in_group_flow_data['data']['collection'][0]['_history_active_flows'];
                                                                    top5_user_max_flow_data=_.max(_history_user_in_group_active_flows_data,
                                                                        function (history_user_in_grp_active_flows) {
                                                                            return history_user_in_grp_active_flows[1];
                                                                        });
                                                                    top5_user_max_flow = top5_user_max_flow_data[1];
                                                                    top5_user_max_flow_time = (new Date(top5_user_max_flow_data[0])).toLocaleString();
                                                                } catch(exception) {
                                                                    top5_user_max_flow = 0; top5_user_max_flow_time = "none";
                                                                }
                                                                top_user_data.push({
                                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top5_user_name:"None",
                                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top5_user_total:0,
                                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top5_user_down:0,
                                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top5_user_up:0,
                                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top5_user_active_flow:0,
                                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top5_user_packet_disc_rate:0,
                                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top5_user_packet_disc:0,
                                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top5_user_max_flow:0,
                                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top5_user_max_flow_time:0,
                                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top_user_from:"None",
                                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top_user_until:"None",
                                                                    "top_user_app1_name": top1_user_app1_name,
                                                                    "top_user_app1_total": top1_user_app1_total,
                                                                    "top_user_app1_from": top_user_from,
                                                                    "top_user_app1_until": top_user_until,
                                                                    "top_user_app2_name": top1_user_app2_name,
                                                                    "top_user_app2_total": top1_user_app2_total,
                                                                    "top_user_app2_from": top_user_from,
                                                                    "top_user_app2_until": top_user_until,
                                                                    "top_user_app3_name": top1_user_app3_name,
                                                                    "top_user_app3_total": top1_user_app3_total,
                                                                    "top_user_app3_from": top_user_from,
                                                                    "top_user_app3_until": top_user_until
                                                                });
                                                                resolve_cnt += 1;
                                                            });
                                                        });
                                                    }
                                                });
                                            } else{
                                                top_user_data.push({
                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top1_user_name:"None",
                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top1_user_total:0,
                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top1_user_down:0,
                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top1_user_up:0,
                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top1_user_active_flow:0,
                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top1_user_packet_disc_rate:0,
                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top1_user_packet_disc:0,
                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top1_user_max_flow:0,
                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top1_user_max_flow_time:0,
                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top1_user_from:"None",
                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top1_user_until:"None",
                                                    "top_user_app1_name": "None",
                                                    "top_user_app1_total": 0,
                                                    "top_user_app1_from": "None",
                                                    "top_user_app1_until": "None",
                                                    "top_user_app2_name": "None",
                                                    "top_user_app2_total": 0,
                                                    "top_user_app2_from": "None",
                                                    "top_user_app2_until": "None",
                                                    "top_user_app3_name": "None",
                                                    "top_user_app3_total": 0,
                                                    "top_user_app3_from": "None",
                                                    "top_user_app3_until": "None"
                                                });
                                                top_user_data.push({
                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top2_user_name:"None",
                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top2_user_total:0,
                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top2_user_down:0,
                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top2_user_up:0,
                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top2_user_active_flow:0,
                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top2_user_packet_disc_rate:0,
                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top2_user_packet_disc:0,
                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top2_user_max_flow:0,
                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top2_user_max_flow_time:0,
                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top2_user_from:"None",
                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top2_user_until:"None",
                                                    "top_user_app1_name": "None",
                                                    "top_user_app1_total": 0,
                                                    "top_user_app1_from": "None",
                                                    "top_user_app1_until": "None",
                                                    "top_user_app2_name": "None",
                                                    "top_user_app2_total": 0,
                                                    "top_user_app2_from": "None",
                                                    "top_user_app2_until": "None",
                                                    "top_user_app3_name": "None",
                                                    "top_user_app3_total": 0,
                                                    "top_user_app3_from": "None",
                                                    "top_user_app3_until": "None"
                                                });
                                                top_user_data.push({
                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top3_user_name:"None",
                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top3_user_total:0,
                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top3_user_down:0,
                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top3_user_up:0,
                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top3_user_active_flow:0,
                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top3_user_packet_disc_rate:0,
                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top3_user_packet_disc:0,
                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top3_user_max_flow:0,
                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top3_user_max_flow_time:0,
                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top3_user_from:"None",
                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top3_user_until:"None",
                                                    "top_user_app1_name": "None",
                                                    "top_user_app1_total": 0,
                                                    "top_user_app1_from": "None",
                                                    "top_user_app1_until": "None",
                                                    "top_user_app2_name": "None",
                                                    "top_user_app2_total": 0,
                                                    "top_user_app2_from": "None",
                                                    "top_user_app2_until": "None",
                                                    "top_user_app3_name": "None",
                                                    "top_user_app3_total": 0,
                                                    "top_user_app3_from": "None",
                                                    "top_user_app3_until": "None"
                                                });
                                                top_user_data.push({
                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top4_user_name:"None",
                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top4_user_total:0,
                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top4_user_down:0,
                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top4_user_up:0,
                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top4_user_active_flow:0,
                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top4_user_packet_disc_rate:0,
                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top4_user_packet_disc:0,
                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top4_user_max_flow:0,
                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top4_user_max_flow_time:0,
                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top4_user_from:"None",
                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top4_user_until:"None",
                                                    "top_user_app1_name": "None",
                                                    "top_user_app1_total": 0,
                                                    "top_user_app1_from": "None",
                                                    "top_user_app1_until": "None",
                                                    "top_user_app2_name": "None",
                                                    "top_user_app2_total": 0,
                                                    "top_user_app2_from": "None",
                                                    "top_user_app2_until": "None",
                                                    "top_user_app3_name": "None",
                                                    "top_user_app3_total": 0,
                                                    "top_user_app3_from": "None",
                                                    "top_user_app3_until": "None"
                                                });
                                                top_user_data.push({
                                                    "top_user_name": (data['data']['collection'].length !== 0) ? top5_user_name:"None",
                                                    "top_user_total": (data['data']['collection'].length !== 0) ? top5_user_total:0,
                                                    "top_user_down": (data['data']['collection'].length !== 0) ? top5_user_down:0,
                                                    "top_user_up": (data['data']['collection'].length !== 0) ? top5_user_up:0,
                                                    "top_user_active_flow": (data['data']['collection'].length !== 0) ? top5_user_active_flow:0,
                                                    "top_user_packet_disc_rate": (data['data']['collection'].length !== 0) ? top5_user_packet_disc_rate:0,
                                                    "top_user_packet_disc": (data['data']['collection'].length !== 0) ? top5_user_packet_disc:0,
                                                    "top_user_max_flow" : (data['data']['collection'].length !== 0) ? top5_user_max_flow:0,
                                                    "top_user_max_flow_time" : (data['data']['collection'].length !== 0) ? top5_user_max_flow_time:0,
                                                    "top_user_from": (data['data']['collection'].length !== 0) ? top5_user_from:"None",
                                                    "top_user_until": (data['data']['collection'].length !== 0) ? top5_user_until:"None",
                                                    "top_user_app1_name": "None",
                                                    "top_user_app1_total": 0,
                                                    "top_user_app1_from": "None",
                                                    "top_user_app1_until": "None",
                                                    "top_user_app2_name": "None",
                                                    "top_user_app2_total": 0,
                                                    "top_user_app2_from": "None",
                                                    "top_user_app2_until": "None",
                                                    "top_user_app3_name": "None",
                                                    "top_user_app3_total": 0,
                                                    "top_user_app3_from": "None",
                                                    "top_user_app3_until": "None"
                                                });
                                            }
                                            _user_in_group_tb.push({
                                                "user_group_name": elem,
                                                "user_group_tr": (elem === _user_group_tb_data[index].name) ? _user_group_tb_data[index].total:"none",
                                                "top_user_data": top_user_data
                                            });
                                        });
                                    };
                                    makeTableData().then(function(res){
                                        if (data['data']['collection'].length !== 0) {
                                            for (var i = 0; i<_user_in_group_tb.length; i++) {
                                                _user_in_group_tb[i]['top_user_data'].sort(function (a, b) {
                                                    console.log('top_user_data: ', a, b);
                                                    return b['top_user_total'] - a['top_user_total']
                                                });
                                            }

                                            _user_in_group_tb.sort(function (a, b) {
                                                return b['user_group_tr'] - a['user_group_tr'];
                                            });
                                        }
                                        console.log('_user_in_group_tb', _user_in_group_tb);
                                        if (data['data']['collection'].length !== 0) {
                                            try {_user_in_group_tr_top1.push((data['data']['collection'][0]['total_rate'] * 0.001).toFixed(3));
                                            } catch(exception) {_user_in_group_tr_top1.push(0);}
                                            try {_user_in_group_tr_top2.push((data['data']['collection'][1]['total_rate'] * 0.001).toFixed(3));
                                            } catch(exception) {_user_in_group_tr_top2.push(0);}
                                            try {_user_in_group_tr_top3.push((data['data']['collection'][2]['total_rate'] * 0.001).toFixed(3));
                                            } catch(exception) {_user_in_group_tr_top3.push(0);}
                                            try {_user_in_group_tr_top4.push((data['data']['collection'][3]['total_rate'] * 0.001).toFixed(3));
                                            } catch(exception) {_user_in_group_tr_top4.push(0);}
                                            try {_user_in_group_tr_top5.push((data['data']['collection'][4]['total_rate'] * 0.001).toFixed(3));
                                            } catch(exception) {_user_in_group_tr_top5.push(0);}
                                        }else{
                                            _user_in_group_tr_top1.push(0);
                                            _user_in_group_tr_top2.push(0);
                                            _user_in_group_tr_top3.push(0);
                                            _user_in_group_tr_top4.push(0);
                                            _user_in_group_tr_top5.push(0);
                                        }
                                        _user_in_group_label.push("["+elem+"]"
                                            + " - ( " +
                                            "1. " + top1_user_name + " , " +
                                            "2. " + top2_user_name + " , " +
                                            "3. " + top3_user_name + " , " +
                                            "4. " + top4_user_name + " , " +
                                            "5. " + top5_user_name +
                                            " )"
                                        );
                                    });
                                });

                            });

                            var _user_in_group_tr_data = [
                                _user_in_group_tr_top1,
                                _user_in_group_tr_top2,
                                _user_in_group_tr_top3,
                                _user_in_group_tr_top4,
                                _user_in_group_tr_top5
                            ];
                            var _user_in_group_series = ["TOP 1", "TOP 2", "TOP 3", "TOP 4", "TOP 5"];
                            var _user_in_group_option = {
                                scales: {
                                    xAxes: [{
                                        ticks: {
                                            fontSize: 12,
                                            fontStyle: "bold"
                                        },
                                        scaleLabel: {
                                            display: true,
                                            fontSize: 14,
                                            labelString: '유저 사용량(Mbit/s)',
                                            fontStyle: "bold"
                                        }
                                    }],
                                    yAxes: [{
                                        pointLabelFontSize : 20,
                                        ticks: {
                                            fontSize: 12,
                                            fontStyle: "bold",
                                            autoSkip: false,
                                            userCallback: function(value, index, values) {
                                                var start = value.indexOf('[');
                                                var end = value.indexOf(']');
                                                var v = value.substr(start+1, end-1);
                                                return v;
                                            }
                                        },
                                        scaleLabel: {
                                            display: true,
                                            fontSize: 14,
                                            labelString: '그룹 내 유저(Top1,Top2,Top3,Top4,Top5)',
                                            fontStyle: "bold"
                                        }

                                    }]
                                },
                                tooltips: {
                                    callbacks: {
                                        title: function(tooltipItems, data) {
                                            return data.labels[tooltipItems[0].index]
                                        }
                                    }
                                }
                            };
                            deferred.resolve({
                                user_group: {
                                    _user_group_label: _user_group_label,
                                    _user_group_data: _user_group_data,
                                    _user_group_series: _user_group_series,
                                    _user_group_option: _user_group_option,
                                    _user_group_colors: _user_group_colors,
                                    _user_group_size: _user_group_size,
                                    _user_group_tb_data: _user_group_tb_data // for table

                                },
                                user_in_group: {
                                    _user_in_group_tb: _user_in_group_tb, // for table
                                    _group_name_tb: _group_name_tb, // for table thead
                                    _user_in_group_tr_data: _user_in_group_tr_data,
                                    _user_in_group_label: _user_in_group_label,
                                    _user_in_group_series: _user_in_group_series,
                                    _user_in_group_option: _user_in_group_option,
                                    _user_in_group_colors: _user_group_colors
                                },
                                complete_count: complete_count
                            });
                        });
                    });
                });
            }
            return deferred.promise;
        };
    };
    return UserGroupData;
});
reportApp.service('ReportUserGroupSize', function($window, $q, ReportData) {
    var UserGroupSize = function() {
        var self = this;
        this.q_userGroupSize = function() {
            var deferred = $q.defer();
            ReportData.getUserGroupSize().then(function (data) {
                var group_size = data.data.size;
                console.log(group_size);

                deferred.resolve({
                    group_size: group_size
                });
            });
            return deferred.promise;
        };
    };
    return UserGroupSize;
});
reportApp.service('SharedData', function() {
    var sharedData = {};
    sharedData.currentDurationState = true;
    sharedData.currentBtnState = false;
    sharedData.currentState = true;
    sharedData.from;
    sharedData.until;
    sharedData.select2model;
    sharedData.report_type;
    sharedData.group_size = 0;

    sharedData.errorCode = {
        user: {
            E01: 'ERROR - 유저 데이터를 가져오지 못했습니다.',
            E02: 'ERROR - 유저 ActiveFlows데이터를 가져오지 못했습니다.',
            E03: 'ERROR - 유저 사이즈를 가져오지 못했습니다.',
            E04: 'ERROR - 유저 패킷 제어양 데이터를 가져오지 못했습니다.',
            E05: 'ERROR - 유저-앱 연관 데이터를 가져오지 못했습니다.',
            W01: 'WARN - 유저 데이터가 존재하지 않습니다.',
            W02: 'WARN - 유저 ActiveFlows데이터가 존재하지 않습니다.',
            W03: 'WARN - 유저 사이즈가 존재하지 않습니다.',
            W04: 'WARN - 유저 패킷 제어양 데이터가 존재하지 않습니다.',
            W05: 'WARN - 유저-앱 연관 데이터가 존재하지 않습니다.'
        },
        interface:{
            E01: 'ERROR - 인터페이스 이름을 가져오지 못했습니다.',
            E02: 'ERROR - 인터페이스 송신 데이터를 가져오지 못했습니다.',
            E03: 'ERROR - 인터페이스 수신 데이터를 가져오지 못했습니다.',
            W01: 'WARN - 인터페이스 이름을 가져오지 못했습니다.',
            W02: 'WARN - 인터페이스 송신 데이터가 존재하지 않습니다.',
            W03: 'WARN - 인터페이스 수신 데이터가 존재하지 않습니다.'
        },
        user_group: {
            E01: 'ERROR - 유저그룹 데이터를 가져오지 못했습니다.',
            E02: 'ERROR - 유저그룹 ActiveFlows 데이터를 가져오지 못했습니다.',
            E03: 'ERROR - 유저 그룹 사이즈를 가져오지 못했습니다.',
            E04: 'ERROR - 그룹 내에 유저의 ActiveFlows 데이터를 가져오지 못했습니다.',
            E05: 'ERROR - 그룹 내에 유저-앱 연관 데이터를 가져오지 못했습니다.',
            W01: 'WARN - 유저그룹 데이터가 존재하지 않습니다.',
            W02: 'WARN - 유저그룹 ActiveFlows 데이터가 존재하지 않습니다.',
            W03: 'WARN - 유저 그룹 사이즈가 존재하지 않습니다.',
            W04: 'WARN - 그룹 내에 유저의 ActiveFlows 데이터가 존재하지 않습니다.',
            W05: 'WARN - 그룹 내에 유저-앱 연관 데이터가 존재하지 않습니다.'
        },
        metadata:{
            E01:  'ERROR - 메타 데이터를 가져오지 못했습니다.',
            W01:  'WARN - 메타 데이터가 존재하지 않습니다.'
        }
    };

    return {
        setCurrentState: function(arg) {
            sharedData.currentState = arg;
        },
        getCurrentState: function() {
            return sharedData.currentState;
        },
        getSharedData: function() {
            return sharedData;
        },
        setFrom: function(from) {
            sharedData.from = from;
        },
        setUntil: function(until) {
            sharedData.until = until;
        },
        getFrom: function() {
            return sharedData.from;
        },
        getUntil: function() {
            return sharedData.until;
        },
        setSelect2model: function(data) {
            sharedData.select2model = data;
        },
        getSelect2model: function() {
            return sharedData.select2model;
        },
        setReportType: function(data) {
            sharedData.report_type = data;
        },
        getReportType: function() {
            return sharedData.report_type;
        },
        setGroupSize: function(size) {
            sharedData.group_size = size;
        },
        getGroupSize: function() {
            return sharedData.group_size;
        },
        getErrorCode: function(){
            return sharedData.errorCode;
        }

    };
});
reportApp.factory('ReportData', function($http, $log, $base64, $window, ReportFrom, ReportUntil, ReportUrl,
                                         ReportQstring, ReportAuth, ReportConfig, SharedData, Notification)
{
    var from = SharedData.getFrom();
    var until = SharedData.getUntil();
    var errorCode = SharedData.getErrorCode();
    console.log("REPORTDATA->from : until -> " + from + ':' + until);
    $.ajaxSetup({
        async: false
    });
    var result;
    var config = (function() {
        $.getJSON("./config/report-config.json", function(d) {
            console.log(d);
            result = d.config;
        });
        return result;
    })();
    $.ajaxSetup({
        async: true
    });
    console.log(config);
    var rest_from = new ReportFrom("").setFrom(from).getFrom();
    var rest_until = new ReportUntil("").setUntil(until).getUntil();
    var headers = new ReportAuth("").addId(config.common.id).addPasswd(config.common.passwd).getAuth();
    function getMetaLink() {
        var meta_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.metapath)
            .addSection("")
            .addQstring("")
            .getUrls();
        return $http({
            method: 'GET',
            url: meta_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.metadata.E01);
                }
            })
    }
    function getInterfaceName(hostname) {
        var int_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(':hostname', hostname))
            .addSection("interfaces/")
            .addQstring("?token=1&order=%3Eactual_direction&with=actual_direction=external,class%3C=ethernet_interface&start=0&limit=10&select=name,type,actual_direction,peer,state,description")
            .getUrls();
        return $http({
            method: 'GET',
            url: int_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E01);
                }
            })
    }
    function getExtInterfaceNameWithSpan(hostname) {
        var int_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(':hostname', hostname))
            .addSection("interfaces/")
            .addQstring("?token=1&order=%3Eactual_direction&with=actual_direction=external,span_port=true,class%3C=ethernet_interface,&start=0&limit=10&select=name,type,actual_direction,state,description,span_port")
            .getUrls();
        return $http({
            method: 'GET',
            url: int_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E01);
                }
            })
    }
    function getIntInterfaceNameWithSpan(hostname) {
        var int_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(':hostname', hostname))
            .addSection("interfaces/")
            .addQstring("?token=1&order=%3Eactual_direction&with=actual_direction=internal,span_port=true,class%3C=ethernet_interface,&start=0&limit=10&select=name,type,actual_direction,state,description,span_port")
            .getUrls();
        return $http({
            method: 'GET',
            url: int_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E01);
                }
            })
    }
    function getIntRcvData(hostname, int_name) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.interface_rcv.attr)
            .addFrom('&from='+rest_from)
            .addOrder('&operation='+config.interface_rcv.operation)
            .addLimit('&history_points='+config.interface_rcv.hist_point)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.interface_rcv.section.replace(":int_name", int_name))
            .addQstring(rest_qstring)
            .getUrls();

        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                console.log(response);
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E03);
                }
            })
    }
    function getIntTrsData(hostname, int_name) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.interface_trs.attr)
            .addFrom('&from='+rest_from)
            .addOrder('&operation='+config.interface_trs.operation)
            .addLimit('&history_points='+config.interface_trs.hist_point)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.interface_trs.section.replace(":int_name", int_name))
            .addQstring(rest_qstring)
            .getUrls();
        console.log(rest_url);
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E02);
                }
            })
    }
    function getUserData(hostname) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.users_tr.attr)
            .addOrder('&order='+config.users_tr.order)
            .addLimit('&limit='+config.users_tr.limit)
            .addWith('&with='+config.users_tr.with)
            .addFrom('&from='+rest_from)
            .addOperation('&operation='+config.users_tr.operation)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.users_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E01);
                }
            })
    }
    function getUserActiveFlows(hostname) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.users_active_flows.attr)
            .addFrom('&from='+rest_from)
            .addOperation('&operation=raw')
            .addHistPoint('&history_points='+config.users_active_flows.hist_point)
            .addLimit('&limit='+config.users_active_flows.limit)
            .addOrder('&order='+config.users_active_flows.order)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.users_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        console.log("get active url : " + rest_url);
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E02);
                }
            })
    }
    function getUserPacketDiscRate(hostname) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.users_tr.attr)
            .addOrder('&order='+config.users_tr.order)
            .addLimit('&limit='+config.users_tr.limit)
            .addWith('&with='+config.users_tr.with)
            .addFrom('&from='+rest_from)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.users_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E03);
                }
            })
    }
    function getUserGroupActiveFlows(hostname, size) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_group_active_flows.attr)
            .addFrom('&from='+rest_from)
            .addOperation('&operation=raw')
            .addHistPoint('&history_points='+config.user_group_active_flows.hist_point)
            .addLimit('&limit='+size)
            .addOrder('&order='+config.user_group_active_flows.order)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_group_active_flows.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user_group.E02);
                }
            })
    }
    function getUserGroupSize(hostname) {
        var rest_qstring = new ReportQstring("")
            .addLimit('?limit=0')
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_group_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(
            function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user_group.E03);
                }
            }
        )
    }
    function getUserGroupData(hostname, group_size) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_group_tr.attr)
            .addOrder('&order='+config.user_group_tr.order)
            .addLimit('&limit='+group_size)
            .addFrom('&from='+rest_from)
            .addOperation('&operation='+config.user_group_tr.operation)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_group_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                if (data.data.collection.length === 0){
                    Notification.error(errorCode.user_group.W01);
                } else {
                    return data;
                }
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user_group.E01);
                }
            })
    }
    return {
        getMetaLink: getMetaLink,
        getInterfaceName: getInterfaceName,
        getExtInterfaceNameWithSpan: getExtInterfaceNameWithSpan,
        getIntInterfaceNameWithSpan: getIntInterfaceNameWithSpan,
        getIntRcvData: getIntRcvData,
        getIntTrsData: getIntTrsData,
        getUserData: getUserData,
        getUserActiveFlows: getUserActiveFlows,
        getUserGroupSize: getUserGroupSize,
        getUserGroupData: getUserGroupData,
        getUserGroupActiveFlows: getUserGroupActiveFlows
    };
});
reportApp.factory('ReportMain', function($http, $log, $base64, $window, ReportFrom, ReportUntil, ReportUrl,
                                               ReportQstring, ReportAuth, ReportConfig, SharedData, Notification)
{
    var from = SharedData.getFrom();
    var until = SharedData.getUntil();
    var errorCode = SharedData.getErrorCode();
    console.log("REPORTDATA->from : until -> " + from + ':' + until);
    $.ajaxSetup({
        async: false
    });
    var result;
    var config = (function() {
        $.getJSON("./config/report-config.json", function(d) {
            console.log(d);
            result = d.config;
        });
        return result;
    })();
    $.ajaxSetup({
        async: true
    });
    console.log(config);
    var rest_from = new ReportFrom("").setFrom(from).getFrom();
    var rest_until = new ReportUntil("").setUntil(until).getUntil();
    var headers = new ReportAuth("").addId(config.common.id).addPasswd(config.common.passwd).getAuth();
    function getMetaLink() {
        var meta_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.metapath)
            .addSection("")
            .addQstring("")
            .getUrls();
        return $http({
            method: 'GET',
            url: meta_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.metadata.E01);
                }
            })
    }
    function getInterfaceName(hostname) {
        var int_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(':hostname', hostname))
            .addSection("interfaces/")
            .addQstring("?token=1&order=%3Eactual_direction&with=actual_direction=external,class%3C=ethernet_interface&start=0&limit=10&select=name,type,actual_direction,state,description")
            .getUrls();
        return $http({
            method: 'GET',
            url: int_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.interface.E01);
                }
            })
    }
    function getUserGroupSize(hostname) {
        var rest_qstring = new ReportQstring("")
            .addLimit('?limit=0')
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_group_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(
            function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user_group.E03);
                }
            }
        )
    }
    function getUserSize(hostname) {
        var rest_qstring = new ReportQstring("")
            .addLimit('?limit=0')
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.users_tr.section)
            .addQstring(rest_qstring)
            .getUrls();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(
            function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E03);
                }
            }
        )
    }
    return {
        getMetaLink: getMetaLink,
        getInterfaceName: getInterfaceName,
        getUserSize: getUserSize,
        getUserGroupSize: getUserGroupSize
    };
});
reportApp.factory('UserAppData', function($http, $log, $base64, $window, ReportConfig, ReportFrom, ReportUntil, ReportUrl, ReportQstring, ReportAuth, SharedData) {
    var from = SharedData.getFrom();
    var until = SharedData.getUntil();
    var errorCode = SharedData.getErrorCode();
    $.ajaxSetup({
        async: false
    });
    var result;
    var config = (function() {
        $.getJSON("./config/report-config.json", function(d) {
            console.log(d);
            result = d.config;
        });
        return result;
    })();
    $.ajaxSetup({
        async: true
    });

    function getUserAppData(hostname, userid) {
        var rest_from = new ReportFrom("")
            .setFrom(from)
            .getFrom();
        var rest_until = new ReportUntil("")
            .setUntil(until)
            .getUntil();
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_app.attr)
            .addOrder('&order='+config.user_app.order)
            .addLimit('&limit='+config.user_app.limit)
            .addWith('&with='+config.user_app.with)
            .addFrom('&from='+rest_from)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_app.section.replace(':userID', userid))
            .addQstring(rest_qstring)
            .getUrls();
        var headers = new ReportAuth("")
            .addId(config.common.id)
            .addPasswd(config.common.passwd)
            .getAuth();

        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                if (data.data.collection.length === 0){
                    Notification.error(errorCode.user.W05);
                } else {
                    return data;
                }
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E05);
                }
            })
    }

    return {
        getUserAppData: getUserAppData
    };
});
reportApp.factory('UserInGroupData', function($http, $log, $base64, $window, ReportConfig, ReportFrom, ReportUntil,
                                              ReportUrl, ReportQstring, ReportAuth, SharedData, Notification) {
    var from = SharedData.getFrom();
    var until = SharedData.getUntil();
    var errorCode = SharedData.getErrorCode();
    $.ajaxSetup({
        async: false
    });
    var result;
    var config = (function() {
        $.getJSON("./config/report-config.json", function(d) {
            console.log(d);
            result = d.config;
        });
        return result;
    })();
    $.ajaxSetup({
        async: true
    });
    var rest_from = new ReportFrom("").setFrom(from).getFrom();
    var rest_until = new ReportUntil("").setUntil(until).getUntil();
    var headers = new ReportAuth("").addId(config.common.id).addPasswd(config.common.passwd).getAuth();
    function getUserInGroupActiveFlows(hostname, user_name) {
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_in_group_active_flows.attr)
            .addFrom('&from='+rest_from)
            .addOperation('&operation=raw')
            .addHistPoint('&history_points='+config.user_in_group_active_flows.hist_point)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_in_group_active_flows.section.replace(":user_name", user_name))
            .addQstring(rest_qstring)
            .getUrls();
        console.log("get user in group active flows url : " + rest_url);
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                return data;
            },
            function onError(response) {
                console.log(response);
                if (response.status < 0) {
                    Notification.error(errorCode.user.E04);
                }
            })
    }
    function getUserInGroupAppData(hostname, userid) {
        var rest_from = new ReportFrom("")
            .setFrom(from)
            .getFrom();
        var rest_until = new ReportUntil("")
            .setUntil(until)
            .getUntil();
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_app.attr)
            .addOrder('&order='+config.user_app.order)
            .addLimit('&limit='+config.user_app.limit)
            .addWith('&with='+config.user_app.with)
            .addFrom('&from='+rest_from)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_app.section.replace(':userID', userid))
            .addQstring(rest_qstring)
            .getUrls();
        var headers = new ReportAuth("")
            .addId(config.common.id)
            .addPasswd(config.common.passwd)
            .getAuth();

        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                if (data.data.collection.length === 0){
                    Notification.error(errorCode.user.W05);
                } else {
                    return data;
                }
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error(errorCode.user.E05);
                }
            })
    }
    function getUserInGroupData(hostname, user_group_name) {
        var rest_from = new ReportFrom("")
            .setFrom(from)
            .getFrom();
        var rest_until = new ReportUntil("")
            .setUntil(until)
            .getUntil();
        var rest_qstring = new ReportQstring("")
            .addSelect('?select='+config.user_in_group_tr.attr)
            .addOrder('&order='+config.user_in_group_tr.order)
            .addLimit('&limit='+config.user_in_group_tr.limit)
            .addOperation('&operation='+config.user_in_group_tr.operation)
            .addFrom('&from='+rest_from)
            .addUntil('&until='+rest_until)
            .getQstring();
        var rest_url = new ReportUrl("")
            .addDefault(config.common.ip, config.common.port, config.common.path.replace(":hostname", hostname))
            .addSection(config.user_in_group_tr.section.replace(':user_group_name', user_group_name))
            .addQstring(rest_qstring)
            .getUrls();
        var headers = new ReportAuth("")
            .addId(config.common.id)
            .addPasswd(config.common.passwd)
            .getAuth();
        return $http({
            method: 'GET',
            url: rest_url,
            headers: headers
        }).
        then(function(data, status, headers, config) {
                console.log(data.data.collection.length);
                if (data.data.collection.length === 0){
                    Notification.error({message: 'WARN - '+user_group_name+'그룹 내에 유저 데이터가 존재하지 않습니다.', delay: 30000});
                    return data;
                } else {
                    return data;
                }
            },
            function onError(response) {
                if (response.status < 0) {
                    Notification.error('ERROR - '+user_group_name+'그룹 내 유저 데이터를 받아 올 수 없습니다.');
                }
            })
    }

    return {
        getUserInGroupActiveFlows: getUserInGroupActiveFlows,
        getUserInGroupAppData: getUserInGroupAppData,
        getUserInGroupData: getUserInGroupData
    };
});