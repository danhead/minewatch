module.exports = function(grunt) {
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),



        copy: {
            dev: {
                files: [
                    {
                        expand: true,
                        cwd: 'websrc',
                        src: ['**', '!config/**', '!css/**'],
                        dest: 'build/'
                    },
                    {
                    	expand: true,
                    	cwd: 'websrc/config',
                    	src: [ '*.js' ],
                    	dest: 'websrc/config/.temp/'
                    }
                ]
            },
            prod: {
            	files: [
                    {
                        expand: true,
                        cwd: 'websrc',
                        src: ['**', '!config/**', '!css/**', '!js/**'],
                        dest: 'build/'
                    }
                ]
            },
            jsvendor: {
            	files: [
	            	{
	            		expand: true,
	            		flatten: true,
	            		cwd: 'bower_components',
	            		src: [
	            			'jquery/**/jquery.min.js',
	            			'modernizr/**/modernizr.js',
	            			'foundation/**/foundation.min.js'
	            		],
	            		dest: 'build/js/vendor/'
	            	}

            	]
            },
            cssvendor: {
            	files: [
            		{
            			expand: true,
            			flatten: true,
            			cwd: 'bower_components',
            			src: [
            				'**/foundation.min.css'
            			],
            			dest: 'build/css/vendor/'
            		}
            	]
            }
        },


        clean: {
            build: {
                src: [ 'build' ]
            },
            temp: {
            	src: [ 'websrc/config/.temp']
            }
        },

        closureCompiler:  {

            options: {
                compilerFile: 'node_modules/closure-compiler-jar/compiler.jar',
                checkModified: true
            },
            minify: {
                files: [
                    {
                        expand: true,
                        cwd: 'websrc/js',
                        src: ['*.js', '!**/*.min.js'],
                        dest: 'build/js/',
                        ext: '.min.js'
                    },
                    {
                    	expand: true,
                    	cwd: 'websrc/config',
                    	src: ['*.js'],
                    	dest: 'websrc/config/.temp/',
                    	ext: '.js'
                    }
                ]
            }
        },
        sass: {
            prod: {
                options: {
                    style: 'compressed'
                },
                files: {
                    'build/css/styles.css':'websrc/css/styles.scss',
                    'websrc/config/.temp/styles.css':'websrc/config/styles.scss'
                }
            },
            dev: {
                files: {
                    'build/css/styles.css':'websrc/css/styles.scss',
                    'websrc/config/.temp/styles.css':'websrc/config/styles.scss'
                }
            }
        },
        autoprefixer: {
            build: {
                expand: true,
                cwd: 'build/css',
                src: [ '*.css' ],
                dest: 'build/css'
            }
        },
        watch: {
            stylesheets: {
                files: 'websrc/css/*.scss',
                tasks: [ 'sass:dev' ]
            },
            copy: {
                files: [ 'websrc/**', '!websrc/css/**', '!websrc/config/**' ],
                tasks: [ 'copy-dev' ]
            },
            config: {
            	files: 'websrc/config/*',
            	tasks: [ 'copy:dev', 'sass:dev', 'htmlbuild', 'clean:temp']
            }
        },
        fileblocks: {
            options: {
                removeAnchors: true
            },
            todos: {
                src: 'build/index.php',
                blocks: {
                    'styles': {
                    	cwd: 'build',
                    	src: [
                    		'css/vendor/*.css',
                    		'css/*.css'
                    	]
                    },
                    'app': {
                    	cwd: 'build',
                    	src: [
                    		'js/vendor/jquery.min.js',
                    		'js/vendor/*.js',
                    		'js/*.js'
                    	]
                    }
                }
            }
        },
        htmlbuild: {
	        dist: {
	            src: 'websrc/config/index.html',
	            dest: 'build/config/',
	            options: {
	                beautify: false,
	                sections: {
	                    css: 'websrc/config/.temp/styles.css',
	                    js: 'websrc/config/.temp/app.js'
	                }
	            }
	        }
   		},
	    shell: {
	        pblbuild: {
	            command: [
	                'cd pebbleapp',
	                'pebble build'
	            ].join('&&')
	        },
	        pblinstall: {
	        	command: [
	        		'cd pebbleapp',
	        		'pebble install'
	        	].join('&&')
	        }
	    }
    });

    grunt.loadNpmTasks('grunt-contrib-copy');
    grunt.loadNpmTasks('grunt-contrib-clean');
    grunt.loadNpmTasks('grunt-closure-tools');
    grunt.loadNpmTasks('grunt-contrib-sass');
    grunt.loadNpmTasks('grunt-autoprefixer');
    grunt.loadNpmTasks('grunt-contrib-watch');
    grunt.loadNpmTasks('grunt-file-blocks');
    grunt.loadNpmTasks('grunt-html-build');
    grunt.loadNpmTasks('grunt-shell');


    grunt.registerTask(
    	'pbl-build',
    	'Builds the Pebble app',
    	['shell:pblbuild']
    );    
    grunt.registerTask(
    	'pbl-install',
    	'Builds and installs the Pebble app',
    	['shell:pblbuild', 'shell:pblinstall']
    );    
    grunt.registerTask(
    	'copy-dev',
    	'Copys all files',
    	['copy:dev', 'copy:jsvendor', 'copy:cssvendor']
    );
    grunt.registerTask(
    	'copy-prod',
    	'Copys all files',
    	['copy:prod', 'copy:jsvendor', 'copy:cssvendor']
    );
    grunt.registerTask(
        'build-dev',
        'Compiles all of the assets and copies the files to the build directory. Does not minify and obfuscate',
        [ 'clean', 'copy-dev', 'sass:dev', 'autoprefixer', 'fileblocks', 'htmlbuild', 'clean:temp']
    );
    grunt.registerTask(
        'build-prod',
        'Compiles all of the assets and copies the files to the build directory.',
        [ 'clean', 'copy-prod', 'closureCompiler', 'sass:prod', 'autoprefixer', 'fileblocks', 'htmlbuild', 'clean:temp']
    );
    grunt.registerTask(
        'default',
        'Runs build-dev',
        [ 'build-dev' ]
    );
};

