'use strict';

/*
 * Déclaration de l'application
 */

/*
 * Déclaration de l'application
 */
angular.module('DomonetApp', 
[
    'ngRoute',
    'gridster',
    'highcharts-ng',
    'ui.unique',
    'DomonetApp.menuModule',
    'DomonetApp.modulesModule',
    'DomonetApp.devicesModule',
    'DomonetApp.chartsModule',
    'DomonetApp.waitingModule'    
])
.config(['$routeProvider', function($routeProvider) 
  { 
      $routeProvider
      .when('/menu', 
      {
          templateUrl: 'pages/menu/index.html',
          controller: 'menuCtrl'
      })
      .when('/modules', 
      {
          templateUrl: 'pages/modules/index.html',
          controller: 'modulesCtrl'
      })
      .when('/devices', 
      {
          templateUrl: 'pages/devices/index.html',
          controller: 'devicesCtrl'
      })
      .when('/config/:id', 
      {
          templateUrl: 'pages/modules/config.html',
          controller: 'configCtrl'
      })
      .when('/charts', 
      {
          templateUrl: 'pages/charts/index.html',
          controller: 'chartsCtrl'
      })
      .otherwise({redirectTo: '/menu'});
  }
]);

/*
 * Déclaration des modules
 */
var menuModule = angular.module('DomonetApp.menuModule',[]);
var waitingModule = angular.module('DomonetApp.waitingModule',[]);

/*
 * Contrôleur indexCtrl
 */
menuModule.controller('menuCtrl', ['$scope', '$rootScope', '$http',
    function ($scope, $rootScope, $http) 
    {
        $rootScope.$broadcast('waiting', true);
        var url = window.location.protocol + "//" + window.location.host + "/web/index.config.js";
        $http({
            method: "GET",
            url: url,
            headers: { 'Content-Type': 'application/json' }
        }).success(function (data) {
            $scope.menu = data.menu;
            $rootScope.$broadcast('waiting', false);
        });
    }
]);

/*
 * Contrôleur waitCtrl
 */
waitingModule.controller('waitingCtrl', ['$scope',
    function ($scope) 
    {
        $scope.wait = true;
        $scope.$on('waiting', function (event, bWait)
        {
          $scope.wait = bWait;
        });
    }
]);
