const express = require('express');
const app = express();
const bodyParser = require('body-parser');
const images = {}; // Store images by clientid

app.use(bodyParser.json({ limit: '50mb' })); // for parsing application/json

app.use(express.static('public'));


// API to receive image base64 from a client
app.post('/api/upload', (req, res) => {
    const { apiKey, image } = req.body;

    // Check if apiKey is valid
    if (apiKey) {
        images[apiKey] = image; // Store image base64 by apiKey
        res.status(200).send('Image uploaded successfully');
        console.log("Image is updated for apikey: " + apiKey)
    } else {
        res.status(400).send('Invalid API key');
    }
});

// Frontend API call to get image to show to HTML page
app.get('/api/image/:clientid', (req, res) => {
    const { clientid } = req.params;

    // Check if image exists for the clientid
    if (images[clientid]) {
        res.status(200).send(images[clientid]);
    } else {
        res.status(404).send('Image not found');
    }
});



const port = 5005;
app.listen(port, () => console.log(`Server is running on port ${port}`));
