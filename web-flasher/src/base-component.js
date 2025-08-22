class BaseComponent extends HTMLElement {
  constructor() {
    super();
    // Attach an open shadow root for encapsulation
    if (!this.shadowRoot) {
      this.attachShadow({ mode: 'open' });
    }
  }

  /**
   * Sub-classes should override to specify which attributes to observe.
   * @returns {string[]}
   */
  static get observedAttributes() {
    return [];
  }

  // Lifecycle hooks — intended to be overridden by sub-classes
  connectedCallback() {}

  disconnectedCallback() {}

  attributeChangedCallback(_name, _oldValue, _newValue) {}

  /**
   * Helper to get a property (stored as a data-* attribute).
   * @param {string} key
   * @returns {string|null}
   */
  getProp(key) {
    return this.getAttribute(`data-${key}`);
  }

  /**
   * Helper to set a property (stored as a data-* attribute).
   * @param {string} key
   * @param {string|null|undefined} value
   */
  setProp(key, value) {
    if (value === null || value === undefined) {
      this.removeAttribute(`data-${key}`);
    } else {
      this.setAttribute(`data-${key}`, value);
    }
  }

  /**
   * Emit a CustomEvent that bubbles by default so parents can listen.
   * @param {string} eventName
   * @param {any} [detail={}]
   * @param {CustomEventInit} [options={}]
   */
  emit(eventName, detail = {}, options = {}) {
    this.dispatchEvent(
      new CustomEvent(eventName, {
        detail,
        bubbles: true,
        composed: true,
        ...options,
      }),
    );
  }

  /**
   * Utility to render a template string into the shadow root.
   * Sub-classes can call this during connectedCallback or whenever state updates.
   * @param {string} templateString
   */
  render(templateString) {
    if (this.shadowRoot) {
      this.shadowRoot.innerHTML = templateString;
    }
  }
}

export default BaseComponent;