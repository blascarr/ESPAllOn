#ifndef _DATA_PROJECTS_HTML_H
#define _DATA_PROJECTS_HTML_H

#include <Arduino.h>

// Simplified embedded HTML for projects page
const char PROJECTS_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESPAllOn - Proyectos</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: Arial, sans-serif; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh; 
            padding: 20px; 
        }
        .container { 
            max-width: 1000px; 
            margin: 0 auto; 
            background: white; 
            border-radius: 15px; 
            box-shadow: 0 20px 40px rgba(0,0,0,0.1); 
            overflow: hidden; 
        }
        .header { 
            background: linear-gradient(135deg, #2c3e50 0%, #34495e 100%); 
            color: white; 
            padding: 30px; 
            text-align: center; 
        }
        .header h1 { font-size: 2.2em; margin-bottom: 10px; }
        .header p { font-size: 1.1em; opacity: 0.9; }
        .controls { 
            padding: 20px; 
            background: #f8f9fa; 
            border-bottom: 1px solid #e9ecef; 
            display: flex; 
            justify-content: space-between; 
            align-items: center; 
            flex-wrap: wrap; 
            gap: 15px; 
        }
        .btn { 
            padding: 10px 20px; 
            border: none; 
            border-radius: 8px; 
            cursor: pointer; 
            font-size: 14px; 
            font-weight: 600; 
            transition: all 0.3s ease; 
            text-decoration: none; 
            display: inline-block; 
        }
        .btn-primary { background: #3498db; color: white; }
        .btn-primary:hover { transform: translateY(-2px); box-shadow: 0 6px 15px rgba(52,152,219,0.3); }
        .btn-success { background: #27ae60; color: white; }
        .btn-success:hover { transform: translateY(-2px); box-shadow: 0 6px 15px rgba(39,174,96,0.3); }
        .btn:disabled { opacity: 0.6; cursor: not-allowed; transform: none !important; }
        .status { 
            padding: 15px 20px; 
            font-weight: 600; 
            text-align: center; 
            border-bottom: 1px solid #e9ecef; 
        }
        .status.loading { background: #fff3cd; color: #856404; }
        .status.success { background: #d4edda; color: #155724; }
        .status.error { background: #f8d7da; color: #721c24; }
        .projects-grid { 
            padding: 20px; 
            display: grid; 
            grid-template-columns: repeat(auto-fill, minmax(300px, 1fr)); 
            gap: 20px; 
        }
        .project-card { 
            border: 2px solid #e9ecef; 
            border-radius: 12px; 
            padding: 20px; 
            transition: all 0.3s ease; 
            cursor: pointer; 
            background: white; 
        }
        .project-card:hover { 
            border-color: #3498db; 
            transform: translateY(-3px); 
            box-shadow: 0 10px 25px rgba(0,0,0,0.1); 
        }
        .project-card.selected { 
            border-color: #27ae60; 
            background: #f8fff9; 
            box-shadow: 0 6px 20px rgba(39,174,96,0.2); 
        }
        .project-title { 
            font-size: 1.3em; 
            font-weight: 700; 
            color: #2c3e50; 
            margin-bottom: 10px; 
        }
        .project-description { 
            color: #7f8c8d; 
            margin: 10px 0; 
            line-height: 1.4; 
        }
        .project-status { 
            padding: 4px 10px; 
            border-radius: 15px; 
            font-size: 0.8em; 
            font-weight: 600; 
            text-transform: uppercase; 
        }
        .status-active { background: #d4edda; color: #155724; }
        .status-completed { background: #cce7ff; color: #004085; }
        .status-inactive { background: #f8d7da; color: #721c24; }
        .btn-load { 
            background:rgb(20, 143, 71); 
            color: white; 
            font-size: 13px; 
            padding: 8px 16px; 
            width: 100%; 
            margin-top: 15px; 
        }
        .btn-load:hover { transform: translateY(-1px); box-shadow: 0 4px 12px rgba(231,76,60,0.3); }
        .empty-state { 
            text-align: center; 
            padding: 40px 20px; 
            color: #7f8c8d; 
        }
        .empty-state h3 { font-size: 1.4em; margin-bottom: 15px; }
        .pagination { 
            padding: 20px; 
            background: #f8f9fa; 
            text-align: center; 
            border-top: 1px solid #e9ecef; 
        }
        .pagination button { 
            margin: 0 5px; 
            padding: 8px 15px; 
            border: 1px solid #3498db; 
            background: white; 
            color: #3498db; 
            border-radius: 5px; 
            cursor: pointer; 
        }
        .pagination button:hover:not(:disabled) { background: #3498db; color: white; }
        .pagination button:disabled { opacity: 0.5; cursor: not-allowed; }
        @media (max-width: 768px) {
            .container { margin: 10px; }
            .header { padding: 20px; }
            .header h1 { font-size: 1.8em; }
            .controls { padding: 15px; flex-direction: column; align-items: stretch; }
            .projects-grid { grid-template-columns: 1fr; padding: 15px; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🚀 Proyectos ESPAllOn</h1>
            <p>Selecciona y carga la configuración del proyecto</p>
        </div>
        <div class="controls">
            <a href="/" class="btn btn-primary">🏠 Panel Principal</a>
            <button id="refreshBtn" class="btn btn-primary" onclick="loadProjects()">
                <span id="refreshIcon">🔄</span> Actualizar
            </button>
        </div>
        <div id="status" class="status" style="display: none"></div>
        <div id="projectsContainer" class="projects-grid">
            <div class="empty-state">
                <h3>👋 ¡Bienvenido!</h3>
                <p>Los proyectos se cargarán automáticamente.</p>
            </div>
        </div>
        <div id="pagination" class="pagination" style="display: none">
            <button id="prevBtn" onclick="goToPreviousPage()" disabled>⬅️ Anterior</button>
            <span id="pageInfo">Página 1</span>
            <button id="nextBtn" onclick="goToNextPage()">Siguiente ➡️</button>
        </div>
    </div>
    <script>
        let projects = [];
        let currentPage = 1;
        let isLoading = false;
        
        function showStatus(message, type) {
            const statusEl = document.getElementById('status');
            statusEl.textContent = message;
            statusEl.className = 'status ' + type;
            statusEl.style.display = 'block';
            if (type !== 'loading') {
                setTimeout(() => statusEl.style.display = 'none', 3000);
            }
        }
        
        function displayProjects(projectsList) {
            const container = document.getElementById('projectsContainer');
            if (!projectsList || projectsList.length === 0) {
                container.innerHTML = '<div class="empty-state"><h3>📭 Sin proyectos</h3><p>No se encontraron proyectos.</p></div>';
                return;
            }
            container.innerHTML = projectsList.map(project => `
                <div class="project-card" onclick="selectProject('${project.id}')">
                    <div class="project-title">${project.name}</div>
                    <div class="project-description">${project.description || 'Sin descripción'}</div>
                    <span class="project-status status-${(project.status || 'unknown').toLowerCase()}">
                        ${getStatusIcon(project.status)} ${project.status || 'UNKNOWN'}
                    </span>
                    <button class="btn btn-load" onclick="event.stopPropagation(); loadProject('${project.id}')">
                        ⚙️ Cargar Configuración
                    </button>
                </div>
            `).join('');
        }
        
        function getStatusIcon(status) {
            switch ((status || '').toLowerCase()) {
                case 'active': return '🟢';
                case 'completed': return '✅';
                case 'inactive': return '🔴';
                case 'pending': return '🟡';
                default: return '❓';
            }
        }
        
        async function loadProjects(page = 1) {
            if (isLoading) return;
            isLoading = true;
            currentPage = page;
            
            const refreshBtn = document.getElementById('refreshBtn');
            const refreshIcon = document.getElementById('refreshIcon');
            refreshBtn.disabled = true;
            refreshIcon.textContent = '⏳';
            
            showStatus('🔄 Cargando proyectos...', 'loading');
            
            try {
                const response = await fetch('/api/projects?page=' + page + '&limit=5');
                if (!response.ok) throw new Error('HTTP error! status: ' + response.status);
                
                const data = await response.json();
                if (data.success && data.data) {
                    projects = data.data;
                    displayProjects(projects);
                    updatePagination();
                    showStatus('✅ ' + projects.length + ' proyectos cargados', 'success');
                } else {
                    throw new Error('Respuesta de API inválida');
                }
            } catch (error) {
                showStatus('❌ Error: ' + error.message, 'error');
                displayProjects([]);
            } finally {
                isLoading = false;
                refreshBtn.disabled = false;
                refreshIcon.textContent = '🔄';
            }
        }
        
        function updatePagination() {
            const pagination = document.getElementById('pagination');
            const prevBtn = document.getElementById('prevBtn');
            const nextBtn = document.getElementById('nextBtn');
            const pageInfo = document.getElementById('pageInfo');
            
            if (projects.length === 0 && currentPage === 1) {
                pagination.style.display = 'none';
                return;
            }
            
            pagination.style.display = 'block';
            prevBtn.disabled = currentPage <= 1;
            nextBtn.disabled = projects.length < 5;
            pageInfo.textContent = 'Página ' + currentPage + ' - ' + projects.length + ' proyectos';
        }
        
        function goToPreviousPage() {
            if (currentPage > 1) loadProjects(currentPage - 1);
        }
        
        function goToNextPage() {
            if (projects.length === 5) loadProjects(currentPage + 1);
        }
        
        function selectProject(projectId) {
            document.querySelectorAll('.project-card').forEach(card => {
                card.classList.remove('selected');
            });
            event.target.closest('.project-card').classList.add('selected');
            
            const project = projects.find(p => p.id === projectId);
            showStatus('✅ Seleccionado: ' + project.name, 'success');
        }
        
        async function loadProject(projectId) {
            const project = projects.find(p => p.id === projectId);
            if (!project) return;
            
            if (!confirm('¿Cargar configuración de "' + project.name + '"?\\n\\n⚠️ Esto sobrescribirá la configuración actual.')) {
                return;
            }
            
            showStatus('⏳ Cargando configuración...', 'loading');
            
            try {
                // Verificar que el proyecto tiene configuración
                if (!project.config) {
                    throw new Error('El proyecto no tiene configuración disponible');
                }

                const response = await fetch('/api/config/load', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(project.config)
                });
                
                if (!response.ok) throw new Error('Error HTTP: ' + response.status);
                
                const result = await response.json();
                if (result.success) {
                    showStatus('🎉 Configuración cargada: ' + project.name, 'success');
                } else {
                    throw new Error(result.message || 'Error desconocido al cargar configuración');
                }
            } catch (error) {
                showStatus('❌ Error: ' + error.message, 'error');
            }
        }
        
        document.addEventListener('DOMContentLoaded', function() {
            loadProjects(1);
        });
    </script>
</body>
</html>
)rawliteral";

#endif
