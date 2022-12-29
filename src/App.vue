<script setup>
import { reactive } from 'vue'
import megaman from './assets/Megaman_completed_data.png'
const title = "Picross";
const imageData = reactive([]);
let appImageHeight = 0;
let appImageWidth = 0;

function getColumnNumbers(n) {
    n--; /*Vue for loop with integer values starts at 1*/
    let count = 0;
    let columnString = '';
    for(let i = 0, len = appImageHeight; i < len; i++){
        if (imageData[i][n] !== 'rgba(255,255,255,1)'){
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
    if (count > 0 && count < appImageHeight) {
        columnString = count.toString();
    }
    if (count === appImageHeight){
        columnString = count.toString();
    }

    return columnString;
};

function getRowNumbers(n) {
    n--; /*Vue for loop with integer values starts at 1*/
    let count = 0;
    let rowString = '';
    for(let i = 0, len = imageData[n].length; i < len; i++){
        if (imageData[n][i] !== 'rgba(255,255,255,1)'){
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
    if (count > 0 && count < imageData[n].length) {
        rowString = count.toString();
    }
    if (count === imageData[n].length){
        rowString = count.toString();
    }
    return rowString;
};

function readImageData(event) {
    let offset;
    let depth = 4;
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
                    rgbaString = "rgba(255,255,255,1)";
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
        appImageWidth = imageWidth;
        appImageHeight = imageHeight;
    }
    fileReader.readAsArrayBuffer(fileList[0]);
}
</script>

<template>
  <input type="file" id="fileInput" @change="readImageData"/>
  <img :src="megaman"/>
  <h1>{{title}}</h1>
  <p>Download the megaman image and use it with the file upload to see the Web Assembly png reader code in action.</p>
  <p>The image will be processed and rows and columns will be show numbers for the non-white pixels in the corresponding row/column</p>
  <p>A simple (and incomplete) png reader was written in C++ and then compiled to Web Assembly.</p>
  <p>The Web Assembly code is what is used to decode the image and get the corresponding pixel colours.</p>
  <p>Full code available at: <a href="https://github.com/frozenrunner/Picross">Picross GitHub Repository</a></p>
    <div class="ui-main-container">
        <div class="ui-column-numbers">
            <div class="ui-number-hint ui-number-hint-column" v-for="n in appImageWidth">{{getColumnNumbers(n)}}</div>
        </div>
        <div class="ui-row-numbers">
            <div class="ui-number-hint ui-number-hint-row" v-for="n in appImageHeight">{{getRowNumbers(n)}}</div>
        </div>
        <div id="gameContainer" class="ui-game-container">
            <template v-for="imageRow in imageData">
                <div class="ui-square-container material-icons" v-for="cell in imageRow" :style="{background: cell}"></div>
            </template>
        </div>
    </div>
</template>

<style scoped>
</style>
