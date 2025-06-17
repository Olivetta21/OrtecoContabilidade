import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.InetAddress;
import java.nio.file.*;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Aplicativo de atualização do Domínio - Versão Java
 * Baseado na versão C++ original
 */
public class AtualizarDominio extends JFrame {
    // Constantes
    private static final String CONFIG_ARQ_LOC = "local.txt";
    private static final String INSTALL_LOC = "C:\\Program Files\\AtualizacoesDominio\\";
    
    // Componentes da UI
    private JTextArea logArea;
    private JProgressBar progressBar;
    private JButton startButton;
    private JLabel statusLabel;
    
    // Classes do sistema
    private SystemInfo sysInfo;
    private Logger logger;
    private Logger attLogger;
    private FileTransfer transferidor;
    private UpdateExecutor executor;
    
    // Variáveis de estado
    private String logLocal;
    private String jaAttLoc;
    private List<String> arquivosTransferidos = new ArrayList<>();
    
    /**
     * Construtor principal da aplicação
     */
    public AtualizarDominio() {
        sysInfo = new SystemInfo();
        setupUI();
    }
    
    /**
     * Configuração da interface gráfica
     */
    private void setupUI() {
        setTitle("Atualizador Domínio");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(700, 500);
        setLocationRelativeTo(null);
        
        // Painel principal
        JPanel mainPanel = new JPanel(new BorderLayout(10, 10));
        mainPanel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        // Área de logo/cabeçalho
        JPanel headerPanel = new JPanel(new BorderLayout());
        JLabel headerLabel = new JLabel("ATUALIZADOR DOMÍNIO", JLabel.CENTER);
        headerLabel.setFont(new Font("Arial", Font.BOLD, 24));
        headerPanel.add(headerLabel, BorderLayout.CENTER);
        
        // Área de texto para logs
        logArea = new JTextArea();
        logArea.setEditable(false);
        logArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        JScrollPane scrollPane = new JScrollPane(logArea);
        scrollPane.setPreferredSize(new Dimension(650, 300));
        
        // Barra de progresso
        progressBar = new JProgressBar(0, 100);
        progressBar.setStringPainted(true);
        
        // Botão de iniciar
        startButton = new JButton("Iniciar Atualização");
        startButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                startButton.setEnabled(false);
                executarAtualizacao();
            }
        });
        
        // Label de status
        statusLabel = new JLabel("Pronto para iniciar", JLabel.CENTER);
        
        // Painel de controles
        JPanel controlPanel = new JPanel(new BorderLayout(5, 5));
        controlPanel.add(progressBar, BorderLayout.CENTER);
        controlPanel.add(startButton, BorderLayout.EAST);
        
        // Montagem do layout
        mainPanel.add(headerPanel, BorderLayout.NORTH);
        mainPanel.add(scrollPane, BorderLayout.CENTER);
        mainPanel.add(controlPanel, BorderLayout.SOUTH);
        mainPanel.add(statusLabel, BorderLayout.SOUTH);
        
        // Aviso
        JLabel warningLabel = new JLabel("NÃO FECHE ESSA JANELA ANTES DE TERMINAR A ATUALIZAÇÃO!", JLabel.CENTER);
        warningLabel.setForeground(Color.RED);
        warningLabel.setFont(new Font("Arial", Font.BOLD, 12));
        mainPanel.add(warningLabel, BorderLayout.NORTH);
        
        setContentPane(mainPanel);
    }
    
    /**
     * Método principal de execução da atualização
     */
    private void executarAtualizacao() {
        // Criar uma thread separada para não bloquear a UI
        SwingWorker<Boolean, String> worker = new SwingWorker<Boolean, String>() {
            @Override
            protected Boolean doInBackground() throws Exception {
                updateStatus("Iniciando atualização...");
                publish("Iniciando processo de atualização...");
                
                if (!inicializar()) {
                    publish("Falha na inicialização!");
                    return false;
                }
                
                // Obtém os caminhos dos arquivos a serem transferidos
                List<String> arqs = new ArrayList<>();
                String execLocal = obterCaminhoExecutaveis();
                
                if (execLocal.isEmpty() || !obterArquivosTransferencia(execLocal, arqs)) {
                    return false;
                }
                
                // Obtém e cria a pasta da nova versão
                String pastaNova = arqs.get(arqs.size() - 1);
                arqs.remove(arqs.size() - 1);
                String versaoNova = INSTALL_LOC + pastaNova + "\\";
                
                if (!configurarPastaNova(versaoNova)) {
                    return false;
                }
                
                // Configura o logger de atualizações
                jaAttLoc = versaoNova + "jaAtualizado.txt";
                attLogger = new Logger(jaAttLoc, sysInfo);
                
                // Verifica arquivos já atualizados
                List<String> jaArqs = new ArrayList<>();
                FileSystem.arqToStrings(jaAttLoc, jaArqs, 2);
                
                // Inicializa o transferidor e o executor de atualizações
                transferidor = new FileTransfer(sysInfo, logger);
                executor = new UpdateExecutor(logger, attLogger);
                
                // Move os arquivos para a nova pasta
                updateStatus("Transferindo arquivos...");
                if (!transferidor.transferirArquivos(arqs, versaoNova, jaArqs, arquivosTransferidos, progressBar)) {
                    return false;
                }
                
                // Executa os arquivos de atualização
                updateStatus("Executando atualizações...");
                publish("\nTRANSFERÊNCIA CONCLUÍDA, AGORA ESPERE\nTODAS AS ATUALIZAÇÕES SEREM EXECUTADAS.");
                logger.gravaLogs("finalizou transferência");
                
                if (!executor.executarAtualizacoes(arquivosTransferidos, jaArqs)) {
                    return false;
                }
                
                // Limpa as versões antigas e finaliza
                updateStatus("Limpando arquivos antigos...");
                publish("Deletando arquivos de instalação antigos...");
                FileSystem.deleteFoldersExcept(INSTALL_LOC, pastaNova);
                
                updateStatus("Atualização concluída com sucesso!");
                publish("\nSISTEMA ATUALIZADO! PODE FECHAR ESSA JANELA E ENTRAR NO SISTEMA AGORA!");
                logger.gravaLogs("fechou o programa.");
                
                return true;
            }
            
            @Override
            protected void process(List<String> chunks) {
                for (String msg : chunks) {
                    appendLog(msg);
                }
            }
            
            @Override
            protected void done() {
                try {
                    boolean success = get();
                    startButton.setEnabled(true);
                    if (success) {
                        JOptionPane.showMessageDialog(AtualizarDominio.this, 
                                "Atualização concluída com sucesso!", 
                                "Sucesso", JOptionPane.INFORMATION_MESSAGE);
                    } else {
                        JOptionPane.showMessageDialog(AtualizarDominio.this,
                                "Falha durante a atualização. Verifique os logs.",
                                "Erro", JOptionPane.ERROR_MESSAGE);
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    appendLog("Erro durante a atualização: " + e.getMessage());
                    updateStatus("Erro durante a atualização");
                }
            }
        };
        
        worker.execute();
    }
    
    /**
     * Adiciona texto à área de log
     */
    private void appendLog(String text) {
        logArea.append(text + "\n");
        logArea.setCaretPosition(logArea.getDocument().getLength());
    }
    
    /**
     * Atualiza o label de status
     */
    private void updateStatus(String status) {
        SwingUtilities.invokeLater(() -> statusLabel.setText(status));
    }
    
    /**
     * Inicializa o sistema
     */
    private boolean inicializar() {
        // Lê as configurações iniciais
        List<String> locais = new ArrayList<>();
        
        if (!carregarConfiguracoes(locais)) {
            return false;
        }
        
        // Inicializa o logger
        logger = new Logger(logLocal, sysInfo);
        logger.gravaLogs("Abriu o Programa");
        
        if (!FileSystem.createDirectory(INSTALL_LOC)) {
            logger.gravaLogs("Não foi possível criar o diretório de instalação");
            appendLog("Diretório de instalação não criado!");
            JOptionPane.showMessageDialog(this, 
                    "Não foi possível criar o diretório de instalação. Verifique se o programa está sendo executado como administrador.", 
                    "Erro", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        
        return true;
    }
    
    /**
     * Carrega as configurações do arquivo local.txt
     */
    private boolean carregarConfiguracoes(List<String> locais) {
        int result = FileSystem.arqToStrings(CONFIG_ARQ_LOC, locais, 2);
        switch (result) {
            case 1:
                appendLog("Erro ao abrir o arquivo: " + CONFIG_ARQ_LOC);
                JOptionPane.showMessageDialog(this, 
                        "Erro ao abrir o arquivo: " + CONFIG_ARQ_LOC, 
                        "Erro", JOptionPane.ERROR_MESSAGE);
                return false;
            case 2:
                appendLog("Sem linhas em: " + CONFIG_ARQ_LOC);
                JOptionPane.showMessageDialog(this, 
                        "O arquivo " + CONFIG_ARQ_LOC + " está vazio!", 
                        "Erro", JOptionPane.ERROR_MESSAGE);
                return false;
            case 0:
                if (Files.exists(Paths.get(locais.get(0)))) {
                    logLocal = locais.get(0) + "logs.txt";
                } else {
                    appendLog("O caminho: " + locais.get(0) + " não foi encontrado.");
                    JOptionPane.showMessageDialog(this, 
                            "O caminho especificado não foi encontrado: " + locais.get(0), 
                            "Erro", JOptionPane.ERROR_MESSAGE);
                    return false;
                }
                break;
        }
        return true;
    }
    
    /**
     * Obtém o caminho do arquivo executaveis.txt
     */
    private String obterCaminhoExecutaveis() {
        List<String> locais = new ArrayList<>();
        switch(FileSystem.arqToStrings(CONFIG_ARQ_LOC, locais, 2)) {
            case 0:
                if (Files.exists(Paths.get(locais.get(0)))) {
                    return locais.get(0) + "executaveis.txt";
                }
                appendLog("O caminho: " + locais.get(0) + " não foi encontrado.");
                break;
            default:
                appendLog("Erro ao ler o arquivo de configuração: " + CONFIG_ARQ_LOC);
                break;
        }
        JOptionPane.showMessageDialog(this, 
                "Não foi possível obter o caminho dos executáveis", 
                "Erro", JOptionPane.ERROR_MESSAGE);
        return "";
    }
    
    /**
     * Obtém a lista de arquivos para transferência
     */
    private boolean obterArquivosTransferencia(String execLocal, List<String> arqs) {
        switch(FileSystem.arqToStrings(execLocal, arqs, 2)) {
            case 1:
                appendLog("Erro ao abrir o arquivo: " + execLocal);
                JOptionPane.showMessageDialog(this, 
                        "Erro ao abrir o arquivo: " + execLocal, 
                        "Erro", JOptionPane.ERROR_MESSAGE);
                return false;
            case 2:
                appendLog("Sem linhas em: " + execLocal);
                JOptionPane.showMessageDialog(this, 
                        "O arquivo " + execLocal + " está vazio!", 
                        "Erro", JOptionPane.ERROR_MESSAGE);
                return false;
            case 0:
                if (arqs.size() < 2) {
                    appendLog("OPS, não foi definido nenhum arquivo de atualização para ser transferido!");
                    JOptionPane.showMessageDialog(this, 
                            "Não foi definido nenhum arquivo de atualização para ser transferido!", 
                            "Erro", JOptionPane.ERROR_MESSAGE);
                    return false;
                }
                // Verifica se todos os arquivos existem, exceto o último (nome da pasta)
                for (int i = 0; i < arqs.size() - 1; i++) {
                    if (!Files.exists(Paths.get(arqs.get(i)))) {
                        appendLog("Arquivo inexistente: " + arqs.get(i));
                        JOptionPane.showMessageDialog(this, 
                                "Arquivo inexistente: " + arqs.get(i), 
                                "Erro", JOptionPane.ERROR_MESSAGE);
                        return false;
                    }
                }
                break;
        }
        return true;
    }
    
    /**
     * Configura a pasta da nova versão
     */
    private boolean configurarPastaNova(String versaoNova) {
        if (Files.exists(Paths.get(versaoNova))) {
            logger.gravaLogs("já tem a pasta da nova versão");
        } else {
            if (!FileSystem.createDirectoryIfNotExists(versaoNova)) {
                logger.gravaLogs("não conseguiu criar a pasta da nova versão.");
                appendLog("Pasta da nova versão não criada!");
                JOptionPane.showMessageDialog(this, 
                        "Não foi possível criar a pasta da nova versão!", 
                        "Erro", JOptionPane.ERROR_MESSAGE);
                return false;
            }
        }
        return true;
    }
    
    /**
     * Método principal
     */
    public static void main(String[] args) {
        try {
            // Define o look and feel para o padrão do sistema
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        SwingUtilities.invokeLater(() -> {
            AtualizarDominio app = new AtualizarDominio();
            app.setVisible(true);
        });
    }
    
    /**
     * Classe para gerenciar informações do sistema e usuário
     */
    private static class SystemInfo {
        private String nomeUser;
        private String nomePC;
        
        public SystemInfo() {
            initializeNames();
        }
        
        private void initializeNames() {
            try {
                nomeUser = System.getProperty("user.name");
                nomePC = InetAddress.getLocalHost().getHostName();
            } catch (Exception e) {
                nomeUser = "Unknown";
                nomePC = "Unknown";
            }
        }
        
        public String getUserName() {
            return nomeUser;
        }
        
        public String getComputerName() {
            return nomePC;
        }
        
        public static String obterHorarioAtual() {
            SimpleDateFormat sdf = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss");
            return sdf.format(new Date());
        }
    }
    
    /**
     * Classe para gerenciar logs
     */
    private static class Logger {
        private String logPath;
        private SystemInfo sysInfo;
        
        public Logger(String path, SystemInfo info) {
            this.logPath = path;
            this.sysInfo = info;
        }
        
        public boolean gravaLogs(String text, int maxTentativas) {
            int tentativas = maxTentativas;
            
            while (tentativas > 0) {
                try (FileWriter fw = new FileWriter(logPath, true);
                     BufferedWriter bw = new BufferedWriter(fw);
                     PrintWriter out = new PrintWriter(bw)) {
                    out.println(sysInfo.getUserName() + " - " + sysInfo.getComputerName() + " - " 
                            + SystemInfo.obterHorarioAtual() + " - " + text);
                    return true;
                } catch (IOException e) {
                    System.err.println("Erro ao abrir o arquivo de log. Tentativas restantes: " + tentativas);
                    tentativas--;
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ie) {
                        Thread.currentThread().interrupt();
                    }
                }
            }
            
            return false;
        }
        
        public boolean gravaLogs(String text) {
            return gravaLogs(text, 3);
        }
        
        public boolean gravaJaAtt(String text, int maxTentativas) {
            int tentativas = maxTentativas;
            
            while (tentativas > 0) {
                try (FileWriter fw = new FileWriter(logPath, true);
                     BufferedWriter bw = new BufferedWriter(fw);
                     PrintWriter out = new PrintWriter(bw)) {
                    out.println(text);
                    return true;
                } catch (IOException e) {
                    tentativas--;
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ie) {
                        Thread.currentThread().interrupt();
                    }
                }
            }
            
            return false;
        }
        
        public boolean gravaJaAtt(String text) {
            return gravaJaAtt(text, 3);
        }
    }
    
    /**
     * Classe para gerenciar o sistema de arquivos
     */
    private static class FileSystem {
        public static boolean createDirectoryIfNotExists(String dir) {
            try {
                Path path = Paths.get(dir);
                if (!Files.exists(path)) {
                    Files.createDirectories(path);
                }
                return true;
            } catch (IOException e) {
                System.out.println("Erro ao criar diretório " + dir + ": " + e.getMessage());
                return false;
            }
        }
        
        public static boolean createDirectory(String dir) {
            try {
                Files.createDirectories(Paths.get(dir));
                return true;
            } catch (IOException e) {
                System.out.println("Erro ao criar diretório " + dir + ": " + e.getMessage());
                return false;
            }
        }
        
        public static void deleteFoldersExcept(String installLoc, String exclude) {
            try {
                Files.list(Paths.get(installLoc))
                    .filter(Files::isDirectory)
                    .forEach(path -> {
                        String folderName = path.getFileName().toString();
                        if (!folderName.equals(exclude)) {
                            System.out.println("Deletando pasta: " + path);
                            try {
                                Files.walk(path)
                                    .sorted(Comparator.reverseOrder())
                                    .map(Path::toFile)
                                    .forEach(File::delete);
                            } catch (IOException e) {
                                System.err.println("Erro ao deletar pasta: " + e.getMessage());
                            }
                        } else {
                            System.out.println("Mantendo pasta: " + path);
                        }
                    });
            } catch (IOException e) {
                System.err.println("Erro ao acessar o sistema de arquivos: " + e.getMessage());
            }
        }
        
        public static int arqToStrings(String arq, List<String> vec, int tentativas) {
            vec.clear();
            
            while (tentativas-- > 0) {
                try (BufferedReader br = new BufferedReader(new FileReader(arq))) {
                    String linha;
                    while ((linha = br.readLine()) != null) {
                        vec.add(linha);
                    }
                    
                    if (vec.isEmpty()) return 2;
                    return 0;
                } catch (IOException e) {
                    if (tentativas > 0) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException ie) {
                            Thread.currentThread().interrupt();
                        }
                        continue;
                    }
                    return 1;
                }
            }
            
            return 1;
        }
        
        public static boolean copyWithProgress(String src, String dest, JProgressBar progressBar) {
            try {
                Path source = Paths.get(src);
                Path destination = Paths.get(dest);
                
                // Verifica se o arquivo já existe e é igual
                if (Files.exists(destination) && arquivosIguais(src, dest)) {
                    System.out.println("Arquivo já existe e é idêntico.");
                    return true;
                }
                
                long fileSize = Files.size(source);
                final AtomicBoolean result = new AtomicBoolean(true);
                
                // Cria diretórios pai se necessário
                Files.createDirectories(destination.getParent());
                
                // Copia o arquivo com progresso
                try (InputStream in = Files.newInputStream(source);
                     OutputStream out = Files.newOutputStream(destination)) {
                    
                    byte[] buffer = new byte[1024 * 1024]; // 1MB buffer
                    long bytesRead = 0;
                    int len;
                    
                    while ((len = in.read(buffer)) > 0) {
                        out.write(buffer, 0, len);
                        bytesRead += len;
                        
                        final int progress = (int) ((100 * bytesRead) / fileSize);
                        SwingUtilities.invokeLater(() -> {
                            progressBar.setValue(progress);
                        });
                    }
                }
                
                SwingUtilities.invokeLater(() -> {
                    progressBar.setValue(100);
                });
                
                return result.get();
            } catch (IOException e) {
                System.err.println("Erro ao copiar arquivo: " + e.getMessage());
                return false;
            }
        }
        
        private static boolean arquivosIguais(String caminho1, String caminho2) {
            System.out.println("Comparando arquivos, aguarde...");
            
            try {
                Path path1 = Paths.get(caminho1);
                Path path2 = Paths.get(caminho2);
                
                // Verifica tamanhos primeiro
                if (Files.size(path1) != Files.size(path2)) {
                    return false;
                }
                
                // Compara conteúdo byte a byte
                try (InputStream is1 = Files.newInputStream(path1);
                     InputStream is2 = Files.newInputStream(path2)) {
                    
                    int data1, data2;
                    do {
                        data1 = is1.read();
                        data2 = is2.read();
                        if (data1 != data2) {
                            return false;
                        }
                    } while (data1 != -1);
                    
                    return true;
                }
            } catch (IOException e) {
                System.err.println("Erro ao comparar arquivos: " + e.getMessage());
                return false;
            }
        }
    }
    
    /**
     * Classe para gerenciar os executáveis
     */
    private static class ExecutableManager {
        public static boolean abrirPrograma(String local) {
            try {
                File file = new File(local);
                ProcessBuilder pb = new ProcessBuilder(local);
                pb.directory(file.getParentFile());
                Process process = pb.start();
                
                // Espera o processo terminar
                int exitCode = process.waitFor();
                System.out.println("Código de saída: " + exitCode);
                
                return exitCode == 0;
            } catch (IOException | InterruptedException e) {
                System.err.println("Falha ao iniciar programa: " + e.getMessage());
                if (e instanceof InterruptedException) {
                    Thread.currentThread().interrupt();
                }
                return false;
            }
        }
    }
      /**
     * Classe para transferência de arquivos
     */
    private static class FileTransfer {
        private Logger logger;
        
        public FileTransfer(SystemInfo info, Logger logMain) {
            this.logger = logMain;
        }
        
        public boolean transferirArquivos(List<String> arquivos,
                                         String pastaDestino,
                                         List<String> arquivosJaAtualizados,
                                         List<String> destinosFinais,
                                         JProgressBar progressBar) {
            
            logger.gravaLogs("iniciou transferência");
            System.out.println("\nMovendo arquivos...\n");
            
            int total = arquivos.size();
            int atual = 1;
            
            for (String origem : arquivos) {
                try {
                    File arquivoOrigem = new File(origem);
                    String nomeArquivo = arquivoOrigem.getName();
                    String destino = pastaDestino + nomeArquivo;
                    
                    logger.gravaLogs("copiando: " + nomeArquivo);
                    System.out.println(nomeArquivo + " - " + atual++ + " de " + total);
                    
                    // Verifica se já está atualizado
                    if (!verificarAtualizacao(nomeArquivo, arquivosJaAtualizados)) {
                        continue;
                    }
                    
                    // Copia o arquivo para o novo diretório
                    if (!FileSystem.copyWithProgress(origem, destino, progressBar)) {
                        logger.gravaLogs("erro ao copiar: " + nomeArquivo);
                        System.out.println("Erro ao copiar o arquivo: " + nomeArquivo);
                        JOptionPane.showMessageDialog(null, 
                                "Erro ao copiar o arquivo: " + nomeArquivo, 
                                "Erro", JOptionPane.ERROR_MESSAGE);
                        return false;
                    }
                    
                    destinosFinais.add(destino);
                } catch (Exception e) {
                    System.out.println("Erro - " + e.getMessage());
                    return false;
                }
            }
            
            return true;
        }
        
        private boolean verificarAtualizacao(String nomeArquivo,
                                           List<String> arquivosJaAtualizados) {
            // Verifica se já está atualizado
            for (String arqAtualizado : arquivosJaAtualizados) {
                if (arqAtualizado.equals(nomeArquivo)) {
                    // Se o arquivo já foi atualizado, pergunta ao usuário se deseja atualizar novamente
                    int resposta = JOptionPane.showConfirmDialog(null,
                            "Foi identificado que o instalador \"" + nomeArquivo +
                            "\" já foi executado uma vez. Deseja executá-lo novamente?",
                            "Confirmação", JOptionPane.YES_NO_OPTION);
                    
                    if (resposta == JOptionPane.YES_OPTION) {
                        // Continua com a atualização
                        return true;
                    } else {
                        logger.gravaLogs("já está atualizado: " + nomeArquivo);
                        return false;
                    }
                }
            }
            
            return true;
        }
    }
    
    /**
     * Classe para execução de atualizações
     */
    private static class UpdateExecutor {
        private Logger logger;
        private Logger attLogger;
        
        public UpdateExecutor(Logger log, Logger logAtt) {
            this.logger = log;
            this.attLogger = logAtt;
        }
        
        public boolean executarAtualizacoes(List<String> arquivos, List<String> arquivosJaAtualizados) {
            for (String arquivo : arquivos) {
                File file = new File(arquivo);
                String nome = file.getName();
                
                int resposta = JOptionPane.showConfirmDialog(null,
                        "O programa de atualização \"" + nome + "\" será executado agora.\nDeseja continuar?",
                        "Confirmação", JOptionPane.YES_NO_OPTION);
                
                if (resposta == JOptionPane.NO_OPTION) {
                    continue;
                }
                
                if (!ExecutableManager.abrirPrograma(arquivo)) {
                    JOptionPane.showMessageDialog(null,
                            "Ocorreram erros ao abrir o arquivo: " + nome + ". Abortando...",
                            "Erro", JOptionPane.ERROR_MESSAGE);
                    return false;
                }
                
                logger.gravaLogs("atualizou: " + nome);
                
                boolean jaAtualizado = false;
                for (String arqAtualizado : arquivosJaAtualizados) {
                    if (arqAtualizado.equals(nome)) {
                        jaAtualizado = true;
                        break;
                    }
                }
                
                if (!jaAtualizado) {
                    attLogger.gravaJaAtt(nome);
                }
            }
            
            return true;
        }
    }
}
