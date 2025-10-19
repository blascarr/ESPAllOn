/**
 * ESPAllOn Projects Page JavaScript
 * ================================
 *
 * Handles project management functionality including:
 * - Loading projects from API
 * - Displaying projects in grid layout
 * - Project selection and configuration loading
 * - Status messages and UI interactions
 */

// Global variables
let projects = [];
let selectedProjectId = null;

/**
 * Show status message to user
 * @param {string} message - Message to display
 * @param {string} type - Type of message: 'loading', 'success', 'error'
 */
function showStatus(message, type = 'loading') {
	const statusEl = document.getElementById('status');
	statusEl.textContent = message;
	statusEl.className = `status ${type}`;
	statusEl.style.display = 'block';

	if (type === 'success' || type === 'error') {
		setTimeout(() => {
			statusEl.style.display = 'none';
		}, 5000);
	}
}

/**
 * Load projects from API endpoint
 * Fetches project list and displays them in the grid
 */
async function loadProjects() {
	const refreshBtn = document.getElementById('refreshBtn');
	const refreshIcon = document.getElementById('refreshIcon');

	refreshBtn.disabled = true;
	refreshIcon.innerHTML = '<div class="loading-spinner"></div>';

	showStatus('Cargando proyectos desde la API...', 'loading');

	try {
		const response = await fetch('/api/projects');

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const data = await response.json();

		if (data.success && data.data) {
			projects = data.data;
			displayProjects(projects);
			showStatus(
				`✅ ${projects.length} proyectos cargados exitosamente`,
				'success'
			);
		} else {
			throw new Error('Respuesta de API inválida');
		}
	} catch (error) {
		console.error('Error loading projects:', error);
		showStatus(`❌ Error al cargar proyectos: ${error.message}`, 'error');
		displayEmptyState();
	} finally {
		refreshBtn.disabled = false;
		refreshIcon.textContent = '🔄';
	}
}

/**
 * Display projects in grid layout
 * @param {Array} projectsList - Array of project objects to display
 */
function displayProjects(projectsList) {
	const container = document.getElementById('projectsContainer');

	if (projectsList.length === 0) {
		displayEmptyState();
		return;
	}

	container.innerHTML = projectsList
		.map(
			(project) => `
                <div class="project-card" onclick="selectProject('${
									project.id
								}')">
                    <div class="project-title">${escapeHtml(project.name)}</div>
                    <div class="project-description">${escapeHtml(
											project.description
										)}</div>
                    
                    <div class="project-meta">
                        <span class="project-status status-${project.status}">
                            ${getStatusIcon(project.status)} ${project.status}
                        </span>
                        <span class="project-date">
                            ${formatDate(project.updated_at)}
                        </span>
                    </div>
                    
                    ${
											project.config && project.config.config
												? `
                        <div class="config-preview">
                            <strong>📋 Configuración:</strong>
                            ${project.config.config
															.slice(0, 3)
															.map(
																(item) => `
                                <div class="config-item">
                                    • ${item.ID || 'Item'}: ${
																	item.ESPinner_Mod || 'N/A'
																}
                                </div>
                            `
															)
															.join('')}
                            ${
															project.config.config.length > 3
																? `
                                <div class="config-item">... y ${
																	project.config.config.length - 3
																} más</div>
                            `
																: ''
														}
                        </div>
                    `
												: ''
										}
                </div>
            `
		)
		.join('');
}

/**
 * Display empty state when no projects are available
 */
function displayEmptyState() {
	const container = document.getElementById('projectsContainer');
	container.innerHTML = `
                <div class="empty-state">
                    <h3>📭 No hay proyectos disponibles</h3>
                    <p>No se encontraron proyectos en la API o hubo un error al cargarlos.</p>
                </div>
            `;
}

/**
 * Select a project and update UI
 * @param {string} projectId - ID of the project to select
 */
function selectProject(projectId) {
	selectedProjectId = projectId;

	// Update visual selection
	document.querySelectorAll('.project-card').forEach((card) => {
		card.classList.remove('selected');
	});

	event.currentTarget.classList.add('selected');

	// Enable load config button
	document.getElementById('loadConfigBtn').disabled = false;

	const project = projects.find((p) => p.id === projectId);
	showStatus(`✅ Proyecto seleccionado: ${project.name}`, 'success');
}

/**
 * Load configuration for the selected project
 * Sends POST request to load project configuration
 */
async function loadSelectedProject() {
	if (!selectedProjectId) {
		showStatus('❌ No hay proyecto seleccionado', 'error');
		return;
	}

	const loadBtn = document.getElementById('loadConfigBtn');
	const loadIcon = document.getElementById('loadIcon');

	loadBtn.disabled = true;
	loadIcon.innerHTML = '<div class="loading-spinner"></div>';

	const project = projects.find((p) => p.id === selectedProjectId);
	showStatus(`Cargando configuración de: ${project.name}...`, 'loading');

	try {
		const response = await fetch(`/api/project/${selectedProjectId}/load`, {
			method: 'POST',
			headers: {
				'Content-Type': 'application/json',
			},
		});

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const result = await response.json();

		if (result.success) {
			showStatus(
				`🎉 Configuración cargada exitosamente: ${project.name}`,
				'success'
			);
		} else {
			throw new Error(result.message || 'Error desconocido');
		}
	} catch (error) {
		console.error('Error loading project config:', error);
		showStatus(`❌ Error al cargar configuración: ${error.message}`, 'error');
	} finally {
		loadBtn.disabled = false;
		loadIcon.textContent = '⚙️';
	}
}

// ========== Utility Functions ==========

/**
 * Escape HTML characters to prevent XSS
 * @param {string} text - Text to escape
 * @returns {string} Escaped HTML
 */
function escapeHtml(text) {
	const div = document.createElement('div');
	div.textContent = text;
	return div.innerHTML;
}

/**
 * Get status icon for project status
 * @param {string} status - Project status
 * @returns {string} Emoji icon for status
 */
function getStatusIcon(status) {
	switch (status) {
		case 'active':
			return '🟢';
		case 'completed':
			return '✅';
		case 'inactive':
			return '🔴';
		default:
			return '⚪';
	}
}

/**
 * Format date string to localized format
 * @param {string} dateString - ISO date string
 * @returns {string} Formatted date
 */
function formatDate(dateString) {
	try {
		const date = new Date(dateString);
		return date.toLocaleDateString('es-ES', {
			year: 'numeric',
			month: 'short',
			day: 'numeric',
		});
	} catch {
		return 'Fecha inválida';
	}
}

// ========== Event Listeners ==========

/**
 * Initialize page when DOM is loaded
 */
document.addEventListener('DOMContentLoaded', function () {
	// Optional: Auto-load projects when page loads
	// loadProjects();
});
