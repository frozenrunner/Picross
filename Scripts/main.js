let imageData = [];
// imageData[0] =  ["gray-dark-square","gray-dark-square",'','','','','','','','','',''];
// imageData[1] =  ["black-square",'','','','','','','','','','',''];
// imageData[2] =  ["black-square",'','',"green-dark-square","green-dark-square","green-dark-square","green-dark-square","green-dark-square","green-dark-square",'','',''];
// imageData[3] =  ["black-square",'',"green-dark-square","green-dark-square","green-light-square","green-light-square","green-light-square","gray-light-square","green-dark-square","green-dark-square",'',''];
// imageData[4] =  ["black-square","green-dark-square","green-dark-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-dark-square","green-dark-square",''];
// imageData[5] =  ["black-square","green-dark-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","green-light-square","yellow-dark-square","yellow-light-square",''];
// imageData[6] =  ["yellow-dark-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square","red-square",''];
// imageData[7] =  ["yellow-dark-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","black-square","gray-light-square"];
// imageData[8] =  ["yellow-dark-square","red-square","red-square","red-square","red-square","black-square","black-square","red-square","red-square","red-square","red-square","gray-light-square"];
// imageData[9] =  ["yellow-dark-square","gray-dark-square","gray-dark-square","red-square","red-square","black-square","black-square","red-square","red-square","gray-dark-square","gray-dark-square","gray-light-square"];
// imageData[10] = ["yellow-dark-square","gray-dark-square","green-light-square","gray-dark-square","red-square","black-square","black-square","red-square","gray-dark-square","green-light-square","gray-dark-square","gray-light-square"];
// imageData[11] = ['',"gray-dark-square","green-dark-square","green-light-square","red-square","black-square","black-square","red-square","green-light-square","green-light-square","gray-dark-square",''];
// imageData[12] = ['','','',"green-dark-square","red-square","black-square","black-square","red-square","green-light-square",'','',''];
// imageData[13] = ['','','','',"red-square","black-square","black-square","red-square",'','','',''];

let picross = {};
picross.viewModel = function(initImageWidth, initImageHeight) {
    let self = this;
    self.imageData = ko.observableArray(imageData);
    self.userBoard = ko.observable(initUserBoard());
    self.checkActive = ko.observable(true);
    self.imageWidth = ko.observable(initImageWidth);
    self.imageHeight = ko.observable(initImageHeight);

    function initUserBoard() {
        let userBoardArrayRows = [];
        for (let i = 0, len = initImageHeight; i < len; i ++) {
            let userBoardArrayColumns = {};
            for (let j = 0, len2 = initImageWidth; j < len2; j++)
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
            if (data[i] !== 'rgba(0,0,0,0)'){
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
            if (imageData[i][data()] !== 'rgba(0,0,0,0)'){
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
//0 - Default
//1 - Reveal
//2 - Not a square
//3 - FAIL
    self.revealSquare = function(row, column) {
        if (self.checkActive()){
            if (self.imageData()[row][column] !== ''){
                self.userBoard()[row][column](1);
            } else {
                self.userBoard()[row][column](3);
            }
        } else {
            self.userBoard()[row][column](2);
        }
    };

    self.activateCross = function() {
        self.checkActive(false);
    }

    self.activateCheck = function() {
        self.checkActive(true);
    }
}

picross.view = function(imageWidth, imageHeight) {
    let ret = {};
    
    ret.model = new picross.viewModel(imageWidth, imageHeight);

    ret.init = function() {
        ko.applyBindings(ret.model, document.getElementById("picross"));
    }
    
    return ret;
};

/*********************WebAssembly Stuff*********************/
let offset;
let depth = 4;
document.getElementById("fileInput").addEventListener("change", function(event){
    let fileList = event.target.files;
    let fileReader = new FileReader();
    fileReader.onload = function(event) {
        let resultArray = new Uint8Array(event.target.result);

        Module.__Z18allocateArraySpacei(event.target.result.byteLength);
        offset = Module.__Z21getFileContentsOffsetv();

        const outBuf = new Uint8Array(Module.buffer, offset, resultArray.length);
        for (let i = 0; i < resultArray.length; i++){
            outBuf[i] = resultArray[i];
        }
        Module.__Z16ProcessImageDatav();

        let pixelVectorSize = Module.__Z18getPixelVectorSizev();
        let pixelVectorStart = Module.__Z20getPixelVectorOffsetv();
        let pixelData = new Uint8Array(Module.buffer, pixelVectorStart, depth*pixelVectorSize);
        let imageWidth = Module.__Z13getImageWidthv();
        let imageHeight = Module.__Z14getImageHeightv();
         
        for (let row = 0; row < imageHeight; row++) {
            let rowData = [];
            for (let col = 0; col < imageWidth; col++) {
                let startingPoint = (row * imageWidth * depth) + (col * depth);
                let opacity = pixelData[startingPoint];
                let red = pixelData[startingPoint + 3];
                let green = pixelData[startingPoint + 2];
                let blue = pixelData[startingPoint + 1];

                let rgbaString = "rgba(" +  red + "," + green + "," + blue;
                if (opacity === 0 || (red === 255 && green === 255 && blue === 255)) {
                    rgbaString = "rgba(0,0,0,0)";
                }
                else {
                    opacity = 1;
                    rgbaString += "," + opacity + ")";
                }
                console.log(rgbaString);
                rowData.push(rgbaString);
            }
            imageData[row] = rowData;
        }
        let gameGrid = document.getElementById("gameContainer");
        gameGrid.style.gridTemplateColumns = "repeat(" + imageWidth + ",1fr)";
        gameGrid.style.gridTemplateRows = "repeat(" + imageHeight + ",1fr)";
        picross.view.instance = picross.view(imageWidth, imageHeight);
        picross.view.instance.init();
    }
    fileReader.readAsArrayBuffer(fileList[0]);
});