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

let picross = {};
picross.viewModel = function() {
    let self = this;
    self.imageData = ko.observableArray(imageData);
    self.userBoard = ko.observable(initUserBoard());
    self.checkActive = ko.observable(true);

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

    self.getRowNumbers = function(data) {
        let count = 0;
        let rowString = '';
        for(let i = 0, len = data.length; i < len; i++){
            if (data[i] !== ''){
                count++;
            }
            else {
                if (count > 0){
                    if (rowString.length > 0){
                        rowString += '    ';
                    }
                    rowString += count.toString();
                    count = 0;
                }
            }
        }
        if (count > 0 && count < data.length) {
            rowString = count.toString();
        }
        if (count === data.length){
            rowString = count.toString();
        }
        return rowString;
    };

    self.getColumnNumbers = function(data) {
        let count = 0;
        let columnString = '';
        for(let i = 0, len = self.imageData().length; i < len; i++){
            if (imageData[i][data()] !== ''){
                count++;
            }
            else {
                if (count > 0){
                    if (columnString.length > 0){
                        columnString += '\r\n';
                    }
                    columnString += count.toString();
                    count = 0;
                }
            }
        }
        if (count > 0 && count < self.imageData().length) {
            columnString = count.toString();
        }
        if (count === self.imageData().length){
            columnString = count.toString();
        }

        return columnString;
    };

    self.revealSquare = function(row, column) {
        if (self.checkActive()){
            if (self.imageData()[row][column] !== ''){
                self.userBoard()[row][column](1);
            }
        }
    };

    self.activateCross = function() {
        self.checkActive(false);
    }

    self.activateCheck = function() {
        self.checkActive(true);
    }
}

picross.view = function() {
    let ret = {};
    
    ret.model = new picross.viewModel();

    ret.init = function() {
        ko.applyBindings(ret.model, document.getElementById("picross"));
    }
    
    return ret;
};

picross.view.instance = picross.view();
picross.view.instance.init();