class BaseComponent extends HTMLElement {
  constructor() {
    super();
    if (!this.shadowRoot) {
      this.attachShadow({ mode: 'open' });
    }
  }

  static get observedAttributes() {
    return [];
  }

  connectedCallback() {}
  disconnectedCallback() {}
  attributeChangedCallback(_name, _old, _new) {}

  getProp(key) {
    return this.getAttribute(`data-${key}`);
  }

  setProp(key, value) {
    if (value === null || value === undefined) {
      this.removeAttribute(`data-${key}`);
    } else {
      this.setAttribute(`data-${key}`, value);
    }
  }

  emit(eventName, detail = {}, options = {}) {
    this.dispatchEvent(new CustomEvent(eventName, { detail, bubbles: true, composed: true, ...options }));
  }

  render(html) {
    if (this.shadowRoot) {
      this.shadowRoot.innerHTML = html;
    }
  }
}

// Expose globally so other scripts can extend
window.BaseComponent = BaseComponent;