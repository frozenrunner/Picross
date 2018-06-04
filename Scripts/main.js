let imageData = [];
imageData[0] =  ["gray-dark-square","gray-dark-square",'','','','','','','','','',''];
imageData[1] =  ["black-square",'','','','','','','','','','',''];
imageData[2] =  ["black-square",'','',"green-dark-square","green-dark-square","green-dark-square","green-dark-square","green-dark-square","green-dark-square",'','',''];
imageData[3] =  ["black-square",'',"green-dark-square","green-dark-square","green-light-square","green-light-square","green-light-square","gray-light-square","green-dark-square","green-dark-square",'',''];
imageData[4] =  ["black-square","green-dark-square","green-dark-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-dark-square","green-dark-square",''];
imageData[5] =  ["black-square","green-dark-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","yellow-dark-square","yellow-light-square",''];
imageData[6] =  ["yellow-dark-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square",''];
imageData[7] =  ["yellow-dark-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","gray-light-square"];
imageData[8] =  ["yellow-dark-square","red-square","red-square","red-square","red-square","black-square","black-square","red-square","red-square","red-square","red-square","gray-light-square"];
imageData[9] =  ["yellow-dark-square","gray-dark-square","gray-dark-square","red-square","red-square","black-square","black-square","red-square","red-square","gray-dark-square","gray-dark-square","gray-light-square"];
imageData[10] = ["yellow-dark-square","gray-dark-square","green-light-square","gray-dark-square","red-square","black-square","black-square","red-square","gray-dark-square","green-light-square","gray-dark-square","gray-light-square"];
imageData[11] = ['',"gray-dark-square","green-dark-square","green-light-square","red-square","black-square","black-square","red-square","green-light-square","green-light-square","gray-dark-square",''];
imageData[12] = ['','','',"green-dark-square","red-square","black-square","black-square","red-square","green-light-square",'','',''];
imageData[13] = ['','','','',"red-square","black-square","black-square","red-square",'','','',''];

var ViewModel = function() {
    let self = this;
    self.imageData = ko.observableArray(imageData);
    self.userBoard = ko.observable(initUserBoard());

    function initUserBoard() {
        let userBoardArrayRows = [];
        for (let i = 0, len = 14; i < len; i ++) {
            let userBoardArrayColumns = {};
            for (let j = 0, len2 = 12; j < len2; j++)
            {
                userBoardArrayColumns[j] = ko.observable(0);
            }
            userBoardArrayRows.push(userBoardArrayColumns);
        }
        return userBoardArrayRows;
    }

    self.getColumnNumbers = function(data) {
        let count = 0;
        for(let i = 0, len = data.length; i < len; i++){
            if (data[i] !== ''){
                count++;
            }
        }
        return count;
    }

    self.getRowNumbers = function(data) {
        let count = 0;
        for(let i = 0, len = self.imageData().length; i < len; i++){
            if (imageData[i][data()] !== ''){
                count++;
            }
        }
        return count;
    }

    self.revealSquare = function(row, column) {
        if (self.imageData()[row][column] !== ''){
            self.userBoard()[row][column](1);
        }
    }
}

ko.applyBindings(ViewModel);