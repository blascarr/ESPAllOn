// ========== Global Variables ==========

let projects = [];
let allProjects = [];
let currentPage = 1;
let isLoading = false;

let selectedProjectId = null;
let totalPages = 1;
let totalProjects = 0;

// ========== UI Functions ==========

/**
 * Show status message
 * @param {string} message - Message to display
 * @param {string} type - Type: 'loading', 'success', 'error'
 */
function showStatus(message, type) {
	const statusEl = document.getElementById('status');
	statusEl.textContent = message;
	statusEl.className = `status ${type}`;
	statusEl.style.display = 'block';

	// Auto-hide non-loading messages
	if (type !== 'loading') {
		setTimeout(() => {
			statusEl.style.display = 'none';
		}, 3000);
	}
}

/**
 * Display projects in the grid
 * @param {Array} projectsList - Array of project objects
 */
function displayProjects(projectsList) {
	const container = document.getElementById('projectsContainer');

	if (!projectsList || projectsList.length === 0) {
		displayEmptyState();
		return;
	}

	container.innerHTML = projectsList
		.map(
			(project) => `
                <div class="project-card" onclick="selectProject('${
									project.id
								}')">
                    <div class="project-title">${project.name}</div>
                    <div class="project-description">${
											project.description || 'Sin descripción'
										}</div>
                    <span class="project-status status-${(
											project.status || 'unknown'
										).toLowerCase()}">
                        ${getStatusIcon(project.status)} ${
				project.status || 'UNKNOWN'
			}
                    </span>
                    <button class="btn btn-load" onclick="event.stopPropagation(); loadProject('${
											project.id
										}')">
                        ⚙️ Cargar Configuración
                    </button>
                </div>
            `
		)
		.join('');
}

/**
 * Display empty state
 */
function displayEmptyState() {
	const container = document.getElementById('projectsContainer');
	container.innerHTML = `
		<div class="empty-state">
			<h3>📭 No hay proyectos</h3>
			<p>No se encontraron proyectos en esta página.</p>
		</div>
	`;
}

/**
 * Get status icon for project
 * @param {string} status - Project status
 * @returns {string} Icon emoji
 */
function getStatusIcon(status) {
	switch (status?.toLowerCase()) {
		case 'active':
			return '🟢';
		case 'completed':
			return '✅';
		case 'inactive':
			return '🔴';
		case 'pending':
			return '🟡';
		default:
			return '❓';
	}
}

/**
 * Format project date for display
 * @param {string} dateString - Date string from API
 * @returns {string} Formatted date
 */
function formatProjectDate(dateString) {
	if (!dateString) {
		return 'Fecha no disponible';
	}

	try {
		// Try to parse the date
		const date = new Date(dateString);

		// Check if date is valid
		if (isNaN(date.getTime())) {
			return dateString; // Return original if can't parse
		}

		// Format date in Spanish locale
		const options = {
			year: 'numeric',
			month: 'long',
			day: 'numeric',
			timeZone: 'Europe/Madrid',
		};

		return date.toLocaleDateString('es-ES', options);
	} catch (error) {
		console.warn('Error formatting date:', dateString, error);
		return dateString; // Return original string if formatting fails
	}
}

/**
 * Format configuration data for modal display
 * @param {Object} config - Configuration object
 * @returns {string} Formatted HTML string
 */
function formatConfigForModal(config) {
	if (!config || typeof config !== 'object') {
		return '<p><em>Configuración no válida</em></p>';
	}

	// If config has a 'config' property with an array (ESPinner format)
	if (config.config && Array.isArray(config.config)) {
		return `
			<div class="config-items">
				${config.config
					.map(
						(item, index) => `
					<div class="config-item">
						<div class="config-item-header">
							<strong>📌 Elemento ${index + 1}</strong>
						</div>
						<div class="config-item-details">
							${Object.entries(item)
								.map(
									([key, value]) => `
								<div class="config-detail">
									<span class="config-key">${key}:</span>
									<span class="config-value">${
										Array.isArray(value) ? value.join(', ') : value
									}</span>
								</div>
							`
								)
								.join('')}
						</div>
					</div>
				`
					)
					.join('')}
			</div>
		`;
	}

	// If config is a simple object
	return `
		<div class="config-simple">
			${Object.entries(config)
				.map(
					([key, value]) => `
				<div class="config-detail">
					<span class="config-key">${key}:</span>
					<span class="config-value">${
						Array.isArray(value)
							? value.join(', ')
							: typeof value === 'object'
							? JSON.stringify(value, null, 2)
							: value
					}</span>
				</div>
			`
				)
				.join('')}
		</div>
	`;
}

// ========== API Functions ==========

/**
 * Load projects from API with pagination
 * @param {number} page - Page number (default: 1)
 * @param {boolean} showLoadingInButton - Show loading in refresh button
 */
async function loadProjects(page = 1, showLoadingInButton = true) {
	if (isLoading) {
		console.log('Already loading, skipping request');
		return;
	}

	isLoading = true;
	currentPage = page;

	// UI Loading state
	const refreshBtn = document.getElementById('refreshBtn');
	const refreshIcon = document.getElementById('refreshIcon');

	if (showLoadingInButton && refreshBtn) {
		refreshBtn.disabled = true;
		refreshIcon.textContent = '⏳';
	}

	showStatus('🔄 Cargando proyectos...', 'loading');

	try {
		const limit = 5; // Fixed limit matching PROJECT_LIMIT_QUERY
		const response = await fetch(`/api/projects?page=${page}&limit=${limit}`);

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const data = await response.json();
		console.log('=== API RESPONSE ===');
		console.log('Full response:', JSON.stringify(data, null, 2));

		if (data.success && data.data) {
			projects = data.data;
			console.log(`Loaded ${projects.length} projects for page ${page}`);

			// Simple pagination logic
			if (projects.length < limit) {
				// If we got fewer projects than requested, this is the last page
				totalPages = page;
				console.log('This appears to be the last page');
			} else {
				// If we got exactly 'limit' projects, there might be more
				totalPages = page + 1;
				console.log('There might be more pages');
			}

			// Use API pagination info if available
			if (data.pagination) {
				totalPages = data.pagination.totalPages || totalPages;
				totalProjects = data.pagination.total || projects.length;
				console.log('Using API pagination info:', data.pagination);
			} else if (data.total) {
				totalProjects = data.total;
				totalPages = Math.ceil(totalProjects / limit);
				console.log('Calculated pagination from total:', data.total);
			}

			displayProjects(projects);

			// Reset loading state before updating pagination controls
			isLoading = false;
			updatePaginationControls();

			showStatus(
				`✅ ${projects.length} proyectos cargados (página ${page})`,
				'success'
			);
		} else {
			throw new Error('Respuesta de API inválida');
		}
	} catch (error) {
		console.error('Error loading projects:', error);
		isLoading = false; // Reset loading state on error too
		showStatus(`❌ Error: ${error.message}`, 'error');
		displayEmptyState();
		hidePaginationControls();
	} finally {
		// isLoading is already reset above, but keep this as safety net
		isLoading = false;
		if (showLoadingInButton && refreshBtn) {
			refreshBtn.disabled = false;
			refreshIcon.textContent = '🔄';
		}
	}
}

// ========== Pagination Functions ==========

/**
 * Update pagination controls
 */
function updatePaginationControls() {
	const paginationContainer = document.getElementById('paginationContainer');
	const prevBtn = document.getElementById('prevPageBtn');
	const nextBtn = document.getElementById('nextPageBtn');
	const pageInfo = document.getElementById('pageInfo');

	console.log(`=== PAGINATION UPDATE ===`);
	console.log(
		`Page: ${currentPage}, Projects: ${projects.length}, Loading: ${isLoading}`
	);

	// Hide pagination if no projects and on first page
	if (projects.length === 0 && currentPage === 1) {
		paginationContainer.style.display = 'none';
		return;
	}

	// Show pagination
	paginationContainer.style.display = 'flex';

	// Update buttons
	prevBtn.disabled = currentPage <= 1 || isLoading;

	// Next button logic: only disable if we're sure there are no more pages
	// If we got exactly 5 projects, there might be more pages
	if (projects.length === 5) {
		nextBtn.disabled = isLoading; // Enable if we got a full page
	} else {
		nextBtn.disabled = true; // Disable if we got less than full page (last page)
	}

	// Force remove disabled attribute if it should be enabled
	if (!nextBtn.disabled) {
		nextBtn.removeAttribute('disabled');
	}

	console.log(
		`Projects loaded: ${projects.length}, Next button disabled: ${nextBtn.disabled}, isLoading: ${isLoading}`
	);

	// Update page info
	pageInfo.textContent = `Página ${currentPage} - ${projects.length} proyectos`;

	// Update page numbers
	updatePageNumbers();

	console.log(`Buttons: prev=${prevBtn.disabled}, next=${nextBtn.disabled}`);
	console.log('=== END PAGINATION ===');
}

/**
 * Update page number buttons
 */
function updatePageNumbers() {
	const pageNumbers = document.getElementById('pageNumbers');
	pageNumbers.innerHTML = '';

	// Show current page
	addPageButton(currentPage);

	// Show page 1 if not current
	if (currentPage > 1) {
		pageNumbers.innerHTML = '';
		addPageButton(1);
		if (currentPage > 2) {
			addEllipsis();
		}
		addPageButton(currentPage);
	}

	// Show next page if we have full results
	if (projects.length === 5) {
		addPageButton(currentPage + 1);
	}
}

/**
 * Add page button
 * @param {number} pageNum - Page number
 */
function addPageButton(pageNum) {
	const pageNumbers = document.getElementById('pageNumbers');
	const button = document.createElement('button');
	button.className = `page-btn ${pageNum === currentPage ? 'active' : ''}`;
	button.textContent = pageNum;
	button.onclick = () => goToPage(pageNum);
	pageNumbers.appendChild(button);
}

/**
 * Add ellipsis
 */
function addEllipsis() {
	const pageNumbers = document.getElementById('pageNumbers');
	const ellipsis = document.createElement('span');
	ellipsis.className = 'page-ellipsis';
	ellipsis.textContent = '...';
	pageNumbers.appendChild(ellipsis);
}

/**
 * Hide pagination controls
 */
function hidePaginationControls() {
	const paginationContainer = document.getElementById('paginationContainer');
	paginationContainer.style.display = 'none';
}

/**
 * Go to specific page
 * @param {number} page - Page number
 */
function goToPage(page) {
	if (page < 1 || page === currentPage || isLoading) {
		console.log(
			`Navigation blocked: page=${page}, current=${currentPage}, loading=${isLoading}`
		);
		return;
	}
	console.log(`Navigating to page ${page}`);
	loadProjects(page, false);
}

/**
 * Go to previous page
 */
function goToPreviousPage() {
	if (currentPage > 1) {
		goToPage(currentPage - 1);
	}
}

/**
 * Go to next page
 */
function goToNextPage() {
	goToPage(currentPage + 1);
}

// ========== Project Selection and Loading ==========

/**
 * Select a project
 * @param {string} projectId - Project ID
 */
function selectProject(projectId) {
	selectedProjectId = projectId;
	const project = projects.find((p) => p.id === projectId);

	// Update visual selection
	document.querySelectorAll('.project-card').forEach((card) => {
		card.classList.remove('selected');
	});
	event.target.closest('.project-card').classList.add('selected');

	showStatus(`✅ Seleccionado: ${project.name}`, 'success');
}

/**
 * Show load confirmation dialog
 * @param {string} projectId - Project ID
 */
function showLoadConfirmDialog(projectId) {
	const project = projects.find((p) => p.id === projectId);
	if (!project) return;

	// Create modal
	const modal = document.createElement('div');
	modal.className = 'modal-backdrop';
	modal.innerHTML = `
		<div class="modal-dialog">
			<div class="modal-header">
				<h3>⚠️ Confirmar Carga de Configuración</h3>
			</div>
			<div class="modal-body">
				<div class="project-info">
					<h4>📋 Información del Proyecto</h4>
					<p><strong>Nombre:</strong> ${project.name}</p>
					<p><strong>Descripción:</strong> ${
						project.description || 'Sin descripción disponible'
					}</p>
					<p><strong>Estado:</strong> ${getStatusIcon(project.status)} ${
		project.status || 'UNKNOWN'
	}</p>
					<p><strong>Fecha:</strong> ${formatProjectDate(
						project.created_at || project.date || project.updated_at
					)}</p>
				</div>
				
				${
					project.config
						? `
				<div class="config-details">
					<h4>⚙️ Configuración a Cargar</h4>
					<div class="config-preview">
						${formatConfigForModal(project.config)}
					</div>
				</div>
				`
						: `
				<div class="config-details">
					<h4>⚙️ Configuración</h4>
					<p><em>No hay configuración específica disponible para este proyecto.</em></p>
				</div>
				`
				}
				
				<div class="warning-section">
					<p><strong>¿Estás seguro de que quieres cargar esta configuración?</strong></p>
					<p class="warning-text">⚠️ Esta acción sobrescribirá la configuración actual de la placa.</p>
				</div>
			</div>
			<div class="modal-footer">
				<button class="btn btn-secondary" onclick="closeModal()">❌ Cancelar</button>
				<button class="btn btn-success" onclick="loadProject('${projectId}')">⚙️ Cargar Configuración</button>
			</div>
		</div>
	`;

	document.body.appendChild(modal);
	modal.addEventListener('click', (e) => {
		if (e.target === modal) closeModal();
	});
}

/**
 * Close modal dialog
 */
function closeModal() {
	const modal = document.querySelector('.modal-backdrop');
	if (modal) {
		modal.remove();
	}
}

async function loadProject(projectId) {
	// Buscar el proyecto en los datos actuales o en todos los proyectos
	let project = projects.find((p) => p.id === projectId);
	if (!project) {
		project = allProjects.find((p) => p.id === projectId);
	}

	if (!project) {
		showStatus('❌ Error: Proyecto no encontrado', 'error');
		return;
	}

	if (
		!confirm(
			'¿Cargar configuración de "' +
				project.name +
				'"?\n\n⚠️ Esto sobrescribirá la configuración actual.'
		)
	) {
		return;
	}

	showStatus('⏳ Cargando configuración...', 'loading');

	try {
		// Verificar que el proyecto tiene configuración
		if (!project.config) {
			throw new Error('El proyecto no tiene configuración disponible');
		}
		console.log('🔧 Configuración a enviar:', project.config);
		// Enviar configuración al endpoint de la placa
		const response = await fetch('/api/config/load', {
			method: 'POST',
			headers: {
				'Content-Type': 'application/json',
			},
			body: JSON.stringify(project),
		});

		if (!response.ok) {
			throw new Error(`Error HTTP: ${response.status}`);
		}

		const result = await response.json();

		if (result.success) {
			showStatus('🎉 Configuración cargada: ' + project.name, 'success');

			// Log detalles de la configuración enviada
			console.log('📋 Configuración enviada a la placa:', {
				projectId: projectId,
				projectName: project.name,
				config: project.config,
			});

			// Mostrar detalles de la configuración en consola
			if (project.config.config && Array.isArray(project.config.config)) {
				console.log(
					'🔧 Elementos de configuración cargados en la placa:',
					project.config.config.length
				);
				project.config.config.forEach((item, index) => {
					console.log(
						`  ${index + 1}. ${item.ID || 'Sin ID'} (${
							item.ESPinner_Mod || 'Sin módulo'
						})`
					);
				});
			}
		} else {
			throw new Error(
				result.message || 'Error desconocido al cargar configuración'
			);
		}
	} catch (error) {
		showStatus('❌ Error: ' + error.message, 'error');
		console.error('Error cargando configuración:', error);
	}
}

// ========== Debug Function ==========

/**
 * Debug API response - call from browser console
 */
window.debugAPI = async function () {
	try {
		console.log('=== DEBUG API TEST ===');
		const response = await fetch('/api/projects?page=1&limit=5');
		console.log('Response status:', response.status);
		console.log('Response headers:', [...response.headers.entries()]);

		const data = await response.json();
		console.log('Full API Response:', JSON.stringify(data, null, 2));

		console.log('data.success:', data.success);
		console.log(
			'data.data length:',
			data.data ? data.data.length : 'undefined'
		);
		console.log('data.pagination:', data.pagination);
		console.log('data.total:', data.total);
		console.log('=== END DEBUG ===');

		return data;
	} catch (error) {
		console.error('Debug API Error:', error);
	}
};

// ========== Event Listeners ==========

/**
 * Initialize when DOM is loaded
 */
document.addEventListener('DOMContentLoaded', function () {
	console.log('Projects page loaded, fetching initial data...');
	loadProjects(1);
});
