// Auto-register Web Components found in this directory

// Import each component here so Vite can include them automatically
// Future components should follow the pattern: export default class MyComponent extends BaseComponent {}

// Example placeholder imports (commented until components exist)
// import LoraConfig from './lora-config.js';
// import SensorData from './sensor-data.js';
// import DeviceStatus from './device-status.js';
// import UIButton from './ui-button.js';
// import UIForm from './ui-form.js';
// import UIModal from './ui-modal.js';

const components = [
  // { tag: 'lora-config', klass: LoraConfig },
];

components.forEach(({ tag, klass }) => {
  if (!customElements.get(tag)) {
    customElements.define(tag, klass);
  }
});