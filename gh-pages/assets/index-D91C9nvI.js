(function(){const o=document.createElement("link").relList;if(o&&o.supports&&o.supports("modulepreload"))return;for(const t of document.querySelectorAll('link[rel="modulepreload"]'))c(t);new MutationObserver(t=>{for(const n of t)if(n.type==="childList")for(const s of n.addedNodes)s.tagName==="LINK"&&s.rel==="modulepreload"&&c(s)}).observe(document,{childList:!0,subtree:!0});function a(t){const n={};return t.integrity&&(n.integrity=t.integrity),t.referrerPolicy&&(n.referrerPolicy=t.referrerPolicy),t.crossOrigin==="use-credentials"?n.credentials="include":t.crossOrigin==="anonymous"?n.credentials="omit":n.credentials="same-origin",n}function c(t){if(t.ep)return;t.ep=!0;const n=a(t);fetch(t.href,n)}})();let d="transmitter",r=null;const i={};function f(){i.deviceSelector=document.querySelector('input[name="deviceType"]:checked'),i.status=document.getElementById("status"),i.espWebToolsContainer=document.getElementById("espWebToolsContainer")}function p(){return`/${d==="transmitter"?"sender_manifest.json":"receiver_manifest.json"}`}function u(){i.espWebToolsContainer&&(i.espWebToolsContainer.innerHTML="");const e=document.createElement("div");e.className="esp-web-tools-container",e.style.cssText="margin: 20px 0; padding: 20px; border: 2px dashed #ccc; border-radius: 8px; text-align: center;",r=document.createElement("esp-web-install-button");const o=p();r.manifest=o,console.log("Using manifest:",o),l(`Loading manifest: ${o}`,"info"),fetch(o).then(s=>{s.ok?(console.log("Manifest accessible:",s.status),l("Manifest loaded successfully","success")):(console.error("Manifest not accessible:",s.status),l(`Manifest error: ${s.status}`,"error"))}).catch(s=>{console.error("Failed to fetch manifest:",s),l(`Manifest fetch failed: ${s.message}`,"error")}),r.style.cssText=`
    --esp-tools-button-color: #667eea;
    --esp-tools-button-text-color: white;
    --esp-tools-button-border-radius: 8px;
  `;const a=document.createElement("button");a.slot="activate",a.textContent=`Flash ${d==="transmitter"?"Transmitter":"Receiver"} Firmware`,a.className="btn btn-primary btn-lg",a.style.cssText="padding: 15px 30px; font-size: 18px; margin: 10px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; border-radius: 8px; cursor: pointer;";const c=document.createElement("div");c.slot="unsupported",c.innerHTML=`
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>Browser Not Supported</strong><br>
      Please use Google Chrome or Microsoft Edge to flash firmware.
    </div>
  `;const t=document.createElement("div");t.slot="not-allowed",t.innerHTML=`
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>HTTPS Required</strong><br>
      Web Serial requires a secure connection (HTTPS) to work.
    </div>
  `,r.appendChild(a),r.appendChild(c),r.appendChild(t),r.addEventListener("install-start",g),r.addEventListener("install-success",m),r.addEventListener("install-error",b),r.addEventListener("install-progress",h);const n=document.createElement("div");n.className="instructions",n.innerHTML=`
    <h4>Instructions:</h4>
    <ol>
      <li>Connect your ESP32 device to your computer via USB</li>
      <li>Click the "Flash Firmware" button above</li>
      <li>Select the COM port when prompted</li>
      <li>Wait for the flashing process to complete</li>
      <li>Your device will restart with the new firmware</li>
    </ol>
    <p><strong>Note:</strong> Make sure your device is in download mode (hold BOOT button while connecting USB)</p>

    <h4>If Flashing Fails:</h4>
    <ul>
      <li><strong>Check Device:</strong> Ensure ESP32 is properly connected and in download mode</li>
      <li><strong>Try Again:</strong> Click the flash button again</li>
      <li><strong>Refresh Page:</strong> If issues persist, refresh the page</li>
      <li><strong>Different USB Port:</strong> Try a different USB port or cable</li>
    </ul>
  `,e.appendChild(r),e.appendChild(n),i.espWebToolsContainer&&i.espWebToolsContainer.appendChild(e)}function g(e){console.log("Installation started:",e.detail),l("Installation started - connecting to device...","info")}function m(e){console.log("Installation successful:",e.detail),l("Firmware flashed successfully! Device will restart shortly.","success")}function h(e){console.log("Installation progress:",e.detail),e.detail&&e.detail.progress&&l(`Flashing in progress: ${Math.round(e.detail.progress*100)}%`,"info")}function b(e){console.error("Installation failed:",e.detail);const o=e.detail.error||"Unknown error occurred";l(`Installation failed: ${o}`,"error")}function y(){document.querySelectorAll('input[name="deviceType"]').forEach(o=>{o.addEventListener("change",async a=>{d=a.target.value,console.log(`${d} selected`),u(),l(`${d==="transmitter"?"Transmitter":"Receiver"} firmware selected`,"info")})})}function l(e,o="info"){i.status&&(i.status.textContent=e,i.status.className=`status ${o}`),console.log(`[${o.toUpperCase()}] ${e}`)}window.addEventListener("load",async()=>{console.log("SBT PIO Web Flasher page loaded"),console.log("Using ESP Web Tools for reliable firmware flashing"),f(),u(),console.log("Initializing device selection..."),y(),l("Web flasher ready! Select device type and click Flash Firmware.","success"),console.log("Page initialization complete")});
