(function(){const n=document.createElement("link").relList;if(n&&n.supports&&n.supports("modulepreload"))return;for(const t of document.querySelectorAll('link[rel="modulepreload"]'))l(t);new MutationObserver(t=>{for(const o of t)if(o.type==="childList")for(const d of o.addedNodes)d.tagName==="LINK"&&d.rel==="modulepreload"&&l(d)}).observe(document,{childList:!0,subtree:!0});function i(t){const o={};return t.integrity&&(o.integrity=t.integrity),t.referrerPolicy&&(o.referrerPolicy=t.referrerPolicy),t.crossOrigin==="use-credentials"?o.credentials="include":t.crossOrigin==="anonymous"?o.credentials="omit":o.credentials="same-origin",o}function l(t){if(t.ep)return;t.ep=!0;const o=i(t);fetch(t.href,o)}})();let c="transmitter",r=null;const s={};function p(){s.deviceSelector=document.querySelector('input[name="deviceType"]:checked'),s.status=document.getElementById("status"),s.espWebToolsContainer=document.getElementById("espWebToolsContainer")}function f(){return`./${c==="transmitter"?"sender_manifest.json":"receiver_manifest.json"}`}function u(){s.espWebToolsContainer&&(s.espWebToolsContainer.innerHTML="");const e=document.createElement("div");e.className="esp-web-tools-container",e.style.cssText="margin: 20px 0; padding: 20px; border: 2px dashed #ccc; border-radius: 8px; text-align: center;",r=document.createElement("esp-web-install-button");const n=f();r.manifest=n,console.log("Using manifest:",n),r.style.cssText=`
    --esp-tools-button-color: #667eea;
    --esp-tools-button-text-color: white;
    --esp-tools-button-border-radius: 8px;
  `;const i=document.createElement("button");i.slot="activate",i.textContent=`Flash ${c==="transmitter"?"Transmitter":"Receiver"} Firmware`,i.className="btn btn-primary btn-lg",i.style.cssText="padding: 15px 30px; font-size: 18px; margin: 10px; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; border-radius: 8px; cursor: pointer;";const l=document.createElement("div");l.slot="unsupported",l.innerHTML=`
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>Browser Not Supported</strong><br>
      Please use Google Chrome or Microsoft Edge to flash firmware.
    </div>
  `;const t=document.createElement("div");t.slot="not-allowed",t.innerHTML=`
    <div style="color: #721c24; background-color: #f8d7da; border: 1px solid #f5c6cb; padding: 15px; border-radius: 5px; margin: 10px;">
      <strong>HTTPS Required</strong><br>
      Web Serial requires a secure connection (HTTPS) to work.
    </div>
  `,r.appendChild(i),r.appendChild(l),r.appendChild(t),r.addEventListener("install-start",g),r.addEventListener("install-success",m),r.addEventListener("install-error",b),r.addEventListener("install-progress",h);const o=document.createElement("div");o.className="instructions",o.innerHTML=`
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
  `,e.appendChild(r),e.appendChild(o),s.espWebToolsContainer&&s.espWebToolsContainer.appendChild(e)}function g(e){console.log("Installation started:",e.detail),a("Installation started - connecting to device...","info")}function m(e){console.log("Installation successful:",e.detail),a("Firmware flashed successfully! Device will restart shortly.","success")}function h(e){console.log("Installation progress:",e.detail),e.detail&&e.detail.progress&&a(`Flashing in progress: ${Math.round(e.detail.progress*100)}%`,"info")}function b(e){console.error("Installation failed:",e.detail);const n=e.detail.error||"Unknown error occurred";a(`Installation failed: ${n}`,"error")}function v(){document.querySelectorAll('input[name="deviceType"]').forEach(n=>{n.addEventListener("change",async i=>{c=i.target.value,console.log(`${c} selected`),u(),a(`${c==="transmitter"?"Transmitter":"Receiver"} firmware selected`,"info")})})}function a(e,n="info"){s.status&&(s.status.textContent=e,s.status.className=`status ${n}`),console.log(`[${n.toUpperCase()}] ${e}`)}window.addEventListener("load",async()=>{console.log("SBT PIO Web Flasher page loaded"),console.log("Using ESP Web Tools for reliable firmware flashing"),p(),u(),console.log("Initializing device selection..."),v(),a("Web flasher ready! Select device type and click Flash Firmware.","success"),console.log("Page initialization complete")});
