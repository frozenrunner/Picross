let app = new Vue({
    el: "#app",
    data: {
        title: "Picross",
        imageData: [],
        imageHeight: 0,
        imageWidth: 0
    },
    methods: {
        getColumnNumbers: function(n) {
            n--; /*Vue for loop with integer values starts at 1*/
            let count = 0;
            let columnString = '';
            for(let i = 0, len = this.imageHeight; i < len; i++){
                if (this.imageData[i][n] !== 'rgba(0,0,0,0)'){
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
            if (count > 0 && count < this.imageHeight) {
                columnString = count.toString();
            }
            if (count === this.imageHeight){
                columnString = count.toString();
            }
    
            return columnString;
        },

        getRowNumbers: function(n) {
            n--; /*Vue for loop with integer values starts at 1*/
            let count = 0;
            let rowString = '';
            for(let i = 0, len = this.imageData[n].length; i < len; i++){
                if (this.imageData[n][i] !== 'rgba(0,0,0,0)'){
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
            if (count > 0 && count < this.imageData[n].length) {
                rowString = count.toString();
            }
            if (count === this.imageData[n].length){
                rowString = count.toString();
            }
            return rowString;
        }
    }
});

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
            app.$set(app.imageData, row, rowData);
        }
        let gameGrid = document.getElementById("gameContainer");
        gameGrid.style.gridTemplateColumns = "repeat(" + imageWidth + ",1fr)";
        gameGrid.style.gridTemplateRows = "repeat(" + imageHeight + ",1fr)";
        app.imageWidth = imageWidth;
        app.imageHeight = imageHeight;
    }
    fileReader.readAsArrayBuffer(fileList[0]);
});