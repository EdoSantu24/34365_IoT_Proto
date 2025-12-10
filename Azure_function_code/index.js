const https = require('https');


const TTN_REGION  = "eu1";            // e.g., "eu1", "nam1"
const TTN_APP_ID  = "prototest1"; // The ID of your app in TTN
const TTN_API_KEY = "TTN_API_KEY";       // Your TTN API Key (removed for security)

module.exports = async function (context, req) {
    context.log('### V7 FINAL PARSER STARTED ###'); 
    
    try {
        let incomingData = req.body;
        
        // 1. Sanitize: Ensure we have an array of messages
        if (!incomingData) {
            context.res = { status: 400, body: "No data" };
            return;
        }
        if (!Array.isArray(incomingData)) {
            // Convert single object to array so the loop works
            incomingData = [incomingData]; 
        }

        // 2. Loop through each message
        for (const msg of incomingData) {
            
            let plantType = null;
            let deviceId = msg.deviceId;
            let foundSource = "None";


            if (msg.properties && Array.isArray(msg.properties)) {
                
                // Find the item inside the list where name is "Type"
                const typeItem = msg.properties.find(p => p.name === "Type");
                
                if (typeItem) {
                    plantType = typeItem.value;
                    foundSource = "Properties Array";
                }
            } 

            else if (msg.properties && msg.properties.desired && msg.properties.desired.Type !== undefined) {
                plantType = msg.properties.desired.Type;
                foundSource = "Desired Object";
            }

            // 3. Send to TTN
            if (plantType !== null && deviceId) {
                context.log(`-> SUCCESS: Found Type ${plantType} (Source: ${foundSource})`);
                
                // Ensure it is an integer
                if (typeof plantType === 'string') plantType = parseInt(plantType);

                await sendDownlinkToTTN(deviceId, plantType, context);
            } else {
                context.log(`-> FAIL: No 'Type' found for device ${deviceId}`);
                // Debug helper
                if (msg.properties) context.log("-> Properties content:", JSON.stringify(msg.properties));
            }
        }
        
        context.res = { status: 200, body: "Processed" };

    } catch (error) {
        context.log.error("CRITICAL ERROR:", error);
        context.res = { status: 200, body: "Error Handled" }; 
    }
};

function sendDownlinkToTTN(deviceId, value, context) {
    return new Promise((resolve, reject) => {
        // Convert Integer to Base64
        const buffer = Buffer.alloc(1);
        buffer.writeUInt8(value, 0); 
        const payloadBase64 = buffer.toString('base64');

        context.log(`-> TTN: Sending Downlink (Hex: ${value.toString(16)}, Base64: ${payloadBase64})`);

        const ttnPayload = JSON.stringify({
            downlinks: [{
                f_port: 1,
                frm_payload: payloadBase64,
                priority: "NORMAL"
            }]
        });

        const options = {
            hostname: `${TTN_REGION}.cloud.thethings.network`,
            path: `/api/v3/as/applications/${TTN_APP_ID}/devices/${deviceId}/down/push`,
            method: 'POST',
            headers: {
                'Authorization': `Bearer ${TTN_API_KEY}`,
                'Content-Type': 'application/json',
                'Content-Length': ttnPayload.length
            }
        };

        const req = https.request(options, (res) => {
            context.log(`-> TTN RESPONSE CODE: ${res.statusCode}`);
            resolve();
        });

        req.on('error', (e) => {
            context.log.error(`-> TTN CONNECTION ERROR: ${e.message}`);
            resolve(); 
        });

        req.write(ttnPayload);
        req.end();
    });
}